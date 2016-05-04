/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Thomas L. Quarles
Modified: 2000 AlansFixes
**********/
/*
 */

/* CKTload(ckt)
 * this is a driver program to iterate through all the various
 * load functions provided for the circuit elements in the
 * given circuit
 */

#include "ngspice/ngspice.h"
#include "ngspice/smpdefs.h"
#include "ngspice/cktdefs.h"
#include "ngspice/devdefs.h"
#include "ngspice/sperror.h"

#ifdef USE_CUSPICE
#include "ngspice/CUSPICE/CUSPICE.h"
#include "cuda_runtime.h"
#include "cusparse_v2.h"

/* cudaMemcpy MACRO to check it for errors --> CUDAMEMCPYCHECK(name of pointer, dimension, type, status) */
#define CUDAMEMCPYCHECK(a, b, c, d) \
    if (d != cudaSuccess) \
    { \
        fprintf (stderr, "cuCKTload routine...\n") ; \
        fprintf (stderr, "Error: cudaMemcpy failed on %s size of %d bytes\n", #a, (int)(b * sizeof(c))) ; \
        fprintf (stderr, "Error: %s = %d, %s\n", #d, d, cudaGetErrorString (d)) ; \
        return (E_NOMEM) ; \
    }

#endif

#ifdef XSPICE
/* gtri - add - wbk - 11/26/90 - add include for MIF global data */
#include "ngspice/mif.h"
/* gtri - end - wbk - 11/26/90 */
#endif

static int ZeroNoncurRow(SMPmatrix *matrix, CKTnode *nodes, int rownum);

int
CKTload(CKTcircuit *ckt)
{
#ifdef USE_CUSPICE
    cusparseStatus_t cusparseStatus ;
    double alpha, beta ;
    int status ;
    alpha = 1.0 ;
    beta = 0.0 ;
#else
    int size ;
#endif

    int i;
    double startTime;
    CKTnode *node;
    int error;
#ifdef STEPDEBUG
    int noncon;
#endif /* STEPDEBUG */

#ifdef XSPICE
    /* gtri - begin - Put resistors to ground at all nodes */
    /*   SMPmatrix  *matrix; maschmann : deleted , because unused */

    double gshunt;
    int num_nodes;

    /* gtri - begin - Put resistors to ground at all nodes */
#endif

    startTime = SPfrontEnd->IFseconds();

#ifdef USE_CUSPICE
    status = cuCKTflush (ckt) ;
    if (status != 0)
        return (E_NOMEM) ;
#else
    size = SMPmatSize (ckt->CKTmatrix) ;
    for (i = 0 ; i <= size ; i++)
        *(ckt->CKTrhs + i) = 0 ;

    SMPclear (ckt->CKTmatrix) ;
#endif

#ifdef STEPDEBUG
    noncon = ckt->CKTnoncon;
#endif /* STEPDEBUG */

#ifdef USE_CUSPICE
    status = cuCKTnonconUpdateHtoD (ckt) ;
    if (status != 0)
        return (E_NOMEM) ;

    status = cuCKTrhsOldUpdateHtoD (ckt) ;
    if (status != 0)
        return (E_NOMEM) ;
#endif

    for (i = 0; i < DEVmaxnum; i++) {
        if (DEVices[i] && DEVices[i]->DEVload && ckt->CKThead[i]) {
            error = DEVices[i]->DEVload (ckt->CKThead[i], ckt);

#ifdef USE_CUSPICE
            status = cuCKTnonconUpdateDtoH (ckt) ;
            if (status != 0)
                return (E_NOMEM) ;
#endif

            if (ckt->CKTnoncon)
                ckt->CKTtroubleNode = 0;
#ifdef STEPDEBUG
            if (noncon != ckt->CKTnoncon) {
                printf("device type %s nonconvergence\n",
                       DEVices[i]->DEVpublic.name);
                noncon = ckt->CKTnoncon;
            }
#endif /* STEPDEBUG */
            if (error) return(error);
        }
    }

#ifdef USE_CUSPICE
    /* Copy the CKTdiagGmin value to the GPU */
    status = cudaMemcpy (ckt->d_CKTloadOutput + ckt->total_n_values, &(ckt->CKTdiagGmin), sizeof(double), cudaMemcpyHostToDevice) ;
    CUDAMEMCPYCHECK (ckt->d_CKTloadOutput + ckt->total_n_values, 1, double, status)

    /* Performing CSRMV for the Sparse Matrix using CUSPARSE */
    cusparseSetStream((cusparseHandle_t)(ckt->CKTmatrix->CKTcsrmvHandle), ckt->streams[0]);
    cusparseStatus = cusparseDcsrmv ((cusparseHandle_t)(ckt->CKTmatrix->CKTcsrmvHandle),
                                     CUSPARSE_OPERATION_NON_TRANSPOSE,
                                     ckt->CKTmatrix->CKTklunz, ckt->total_n_values + 1,
                                     ckt->total_n_Ptr + ckt->CKTdiagElements,
                                     &alpha, (cusparseMatDescr_t)(ckt->CKTmatrix->CKTcsrmvDescr),
                                     ckt->d_CKTtopologyMatrixCSRx, ckt->d_CKTtopologyMatrixCSRp,
                                     ckt->d_CKTtopologyMatrixCSRj, ckt->d_CKTloadOutput, &beta,
                                     ckt->CKTmatrix->d_CKTkluAx) ;

    if (cusparseStatus != CUSPARSE_STATUS_SUCCESS)
    {
        fprintf (stderr, "CUSPARSE MATRIX Call Error\n") ;
        return (E_NOMEM) ;
    }

    /* Performing CSRMV for the RHS using CUSPARSE */
    cusparseSetStream((cusparseHandle_t)(ckt->CKTmatrix->CKTcsrmvHandle), ckt->streams[1]);
    cusparseStatus = cusparseDcsrmv ((cusparseHandle_t)(ckt->CKTmatrix->CKTcsrmvHandle),
                                     CUSPARSE_OPERATION_NON_TRANSPOSE,
                                     ckt->CKTmatrix->CKTkluN + 1, ckt->total_n_valuesRHS, ckt->total_n_PtrRHS,
                                     &alpha, (cusparseMatDescr_t)(ckt->CKTmatrix->CKTcsrmvDescr),
                                     ckt->d_CKTtopologyMatrixCSRxRHS, ckt->d_CKTtopologyMatrixCSRpRHS,
                                     ckt->d_CKTtopologyMatrixCSRjRHS, ckt->d_CKTloadOutputRHS, &beta,
                                     ckt->CKTmatrix->d_CKTrhs) ;

    if (cusparseStatus != CUSPARSE_STATUS_SUCCESS)
    {
        fprintf (stderr, "CUSPARSE RHS Call Error\n") ;
        return (E_NOMEM) ;
    }

    cuCKTsystemBarrier (ckt) ;

    status = cuCKTsystemDtoH (ckt) ;
    if (status != 0)
        return (E_NOMEM) ;
#endif

#ifdef XSPICE
    /* gtri - add - wbk - 11/26/90 - reset the MIF init flags */

    /* init is set by CKTinit and should be true only for first load call */
    g_mif_info.circuit.init = MIF_FALSE;

    /* anal_init is set by CKTdoJob and is true for first call */
    /* of a particular analysis type */
    g_mif_info.circuit.anal_init = MIF_FALSE;

    /* gtri - end - wbk - 11/26/90 */

    /* gtri - begin - Put resistors to ground at all nodes. */
    /* Value of resistor is set by new "rshunt" option.     */

    if (ckt->enh->rshunt_data.enabled) {
        gshunt = ckt->enh->rshunt_data.gshunt;
        num_nodes = ckt->enh->rshunt_data.num_nodes;
        for (i = 0; i < num_nodes; i++) {
            *(ckt->enh->rshunt_data.diag[i]) += gshunt;
        }
    }

    /* gtri - end - Put resistors to ground at all nodes */
#endif

    if (ckt->CKTmode & MODEDC) {
        /* consider doing nodeset & ic assignments */
        if (ckt->CKTmode & (MODEINITJCT | MODEINITFIX)) {
            /* do nodesets */
            for (node = ckt->CKTnodes; node; node = node->next) {
                if (node->nsGiven) {
                    if (ZeroNoncurRow(ckt->CKTmatrix, ckt->CKTnodes,
                                      node->number)) {
                        ckt->CKTrhs[node->number] = 1.0e10 * node->nodeset *
                                                      ckt->CKTsrcFact;
                        *(node->ptr) = 1e10;
                    } else {
                        ckt->CKTrhs[node->number] = node->nodeset *
                                                      ckt->CKTsrcFact;
                        *(node->ptr) = 1;
                    }
                    /* DAG: Original CIDER fix. If above fix doesn't work,
                     * revert to this.
                     */
                    /*
                     *  ckt->CKTrhs[node->number] += 1.0e10 * node->nodeset;
                     *  *(node->ptr) += 1.0e10;
                     */
                }
            }
        }
        if ((ckt->CKTmode & MODETRANOP) && (!(ckt->CKTmode & MODEUIC))) {
            for (node = ckt->CKTnodes; node; node = node->next) {
                if (node->icGiven) {
                    if (ZeroNoncurRow(ckt->CKTmatrix, ckt->CKTnodes,
                                      node->number)) {
                        /* Original code:
                         ckt->CKTrhs[node->number] += 1.0e10 * node->ic;
                        */
                        ckt->CKTrhs[node->number] = 1.0e10 * node->ic *
                                                      ckt->CKTsrcFact;
                        *(node->ptr) += 1.0e10;
                    } else {
                        /* Original code:
                          ckt->CKTrhs[node->number] = node->ic;
                        */
                        ckt->CKTrhs[node->number] = node->ic*ckt->CKTsrcFact; /* AlansFixes */
                        *(node->ptr) = 1;
                    }
                    /* DAG: Original CIDER fix. If above fix doesn't work,
                     * revert to this.
                     */
                    /*
                     *  ckt->CKTrhs[node->number] += 1.0e10 * node->ic;
                     *  *(node->ptr) += 1.0e10;
                     */
                }
            }
        }
    }

    /* SMPprint(ckt->CKTmatrix, stdout); if you want to debug, this is a
    good place to start ... */

    ckt->CKTstat->STATloadTime += SPfrontEnd->IFseconds()-startTime;
    return(OK);
}

static int
ZeroNoncurRow(SMPmatrix *matrix, CKTnode *nodes, int rownum)
{
    CKTnode     *n;
    double      *x;
    int         currents;

    currents = 0;
    for (n = nodes; n; n = n->next) {
        x = (double *) SMPfindElt(matrix, rownum, n->number, 0);
        if (x) {
            if (n->type == SP_CURRENT)
                currents = 1;
            else
                *x = 0.0;
        }
    }

    return currents;
}
