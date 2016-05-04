/*
 * Copyright (c) 2014, NVIDIA Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation and/or 
 *    other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to 
 *    endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __cplusplus
extern "C" {
#endif

int cuCKTcsrmv (CKTcircuit *) ;
int cuCKTflush (CKTcircuit *) ;
int cuCKTnonconUpdateHtoD (CKTcircuit *) ;
int cuCKTnonconUpdateDtoH (CKTcircuit *) ;
int cuCKTrhsOldFlush (CKTcircuit *) ;
int cuCKTrhsOldUpdateHtoD (CKTcircuit *) ;
int cuCKTrhsOldUpdateDtoH (CKTcircuit *) ;
int cuCKTsetup (CKTcircuit *) ;
int cuCKTsystemBarrier (CKTcircuit *) ;
int cuCKTsystemDtoH (CKTcircuit *) ;
int cuCKTstatesFlush (CKTcircuit *) ;
int cuCKTstate0UpdateHtoD (CKTcircuit *) ;
int cuCKTstate0UpdateDtoH (CKTcircuit *) ;
int cuCKTstate01copy (CKTcircuit *) ;
int cuCKTstatesCircularBuffer (CKTcircuit *) ;
int cuCKTstate123copy (CKTcircuit *) ;
int cuCKTdeltaOldUpdateHtoD (CKTcircuit *) ;
int cuCKTtrunc (CKTcircuit *, double, double *) ;

int cuBSIM4destroy (GENmodel *) ;
int cuBSIM4getic (GENmodel *) ;
int cuBSIM4load (GENmodel *, CKTcircuit *) ;
int cuBSIM4setup (GENmodel *) ;
int cuBSIM4temp (GENmodel *) ;
int cuBSIM4trunc (GENmodel *, CKTcircuit *, double *) ;

int cuCAPdestroy (GENmodel *) ;
int cuCAPgetic (GENmodel *) ;
int cuCAPload (GENmodel *, CKTcircuit *) ;
int cuCAPsetup (GENmodel *) ;
int cuCAPtemp (GENmodel *) ;
int cuCAPtrunc (GENmodel *, CKTcircuit *, double *) ;

int cuINDdestroy (GENmodel *) ;
int cuINDload (GENmodel *, CKTcircuit *) ;
int cuINDsetup (GENmodel *) ;
int cuINDtemp (GENmodel *) ;
int cuINDtrunc (GENmodel *, CKTcircuit *, double *) ;

int cuISRCdestroy (GENmodel *) ;
int cuISRCload (GENmodel *, CKTcircuit *) ;
int cuISRCsetup (GENmodel *) ;
int cuISRCtemp (GENmodel *) ;

int cuMUTdestroy (GENmodel *) ;
int cuMUTload (GENmodel *, CKTcircuit *) ;
int cuMUTsetup (GENmodel *) ;
int cuMUTtemp (GENmodel *) ;

int cuRESdestroy (GENmodel *) ;
int cuRESload (GENmodel *, CKTcircuit *) ;
int cuRESsetup (GENmodel *) ;
int cuREStemp (GENmodel *) ;

int cuVSRCdestroy (GENmodel *) ;
int cuVSRCload (GENmodel *, CKTcircuit *) ;
int cuVSRCsetup (GENmodel *) ;
int cuVSRCtemp (GENmodel *) ;

#ifdef __cplusplus
}
#endif
