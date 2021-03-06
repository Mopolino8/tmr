/*
  This file is part of the package TMR for adaptive mesh refinement.

  Copyright (C) 2015 Georgia Tech Research Corporation.
  Additional copyright (C) 2015 Graeme Kennedy.
  All rights reserved.

  TMR is licensed under the Apache License, Version 2.0 (the "License");
  you may not use this software except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "SolidShellWrapper.h"

SolidShellWrapper::SolidShellWrapper( MITCShell<2> *_shell ){
  shell = _shell;
  shell->incref();
}

SolidShellWrapper::~SolidShellWrapper(){
  shell->decref();
}

const char * SolidShellWrapper::dispNames[] =
  {"u", "v", "w", "rotx", "roty", "rotz"};

const char* SolidShellWrapper::displacementName( int i ){
  return dispNames[i];
}

ElementType SolidShellWrapper::getElementType(){
  return TACS_SHELL; 
}

int SolidShellWrapper::numDisplacements(){ return 3; }

int SolidShellWrapper::numNodes(){ return 8; }

int SolidShellWrapper::numStresses(){ return 8; }

void SolidShellWrapper::addResidual( double time, TacsScalar res[],
                                     const TacsScalar Xpts[],
                                     const TacsScalar vars[],
                                     const TacsScalar dvars[],
                                     const TacsScalar ddvars[] ){
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  shell->addResidual(time, res, X, vars, dvars, ddvars);
}

void SolidShellWrapper::addJacobian( double time, TacsScalar J[],
                                     double alpha, double beta, double gamma,
                                     const TacsScalar Xpts[],
                                     const TacsScalar vars[],
                                     const TacsScalar dvars[],
                                     const TacsScalar ddvars[] ){
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  shell->addJacobian(time, J, alpha, beta, gamma, 
                     X, vars, dvars, ddvars);
}

void SolidShellWrapper::addAdjResProduct( double time, double scale,
                                          TacsScalar dvSens[], int dvLen,
                                          const TacsScalar psi[],
                                          const TacsScalar Xpts[],
                                          const TacsScalar vars[],
                                          const TacsScalar dvars[],
                                          const TacsScalar ddvars[] ){
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  shell->addAdjResProduct(time, scale, dvSens, dvLen,
                          psi, X, vars, dvars, ddvars);
}

TACSConstitutive *SolidShellWrapper::getConstitutive(){
  return shell->getConstitutive();
}

int SolidShellWrapper::getNumGaussPts(){
  return shell->getNumGaussPts();
}

double SolidShellWrapper::getGaussWtsPts( const int num, double pt[] ){
  return shell->getGaussWtsPts(num, pt);
}

TacsScalar SolidShellWrapper::getDetJacobian( const double pt[], 
                                              const TacsScalar Xpts[] ){
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  return shell->getDetJacobian(pt, X);
}

void SolidShellWrapper::getStrain( TacsScalar strain[], 
                                   const double pt[], 
                                   const TacsScalar Xpts[],
                                   const TacsScalar vars[] ){
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  shell->getStrain(strain, pt, X, vars);
}

void SolidShellWrapper::addStrainSVSens( TacsScalar strainSVSens[], 
                                         const double pt[], 
                                         const TacsScalar scale,
                                         const TacsScalar strainSens[], 
                                         const TacsScalar Xpts[],
                                         const TacsScalar vars[] ){  
  TacsScalar X[3*4];
  X[0] = Xpts[0];   X[1] = Xpts[1];   X[2] = Xpts[2];
  X[3] = Xpts[6];   X[4] = Xpts[7];   X[5] = Xpts[8];
  X[6] = Xpts[12];  X[7] = Xpts[13];  X[8] = Xpts[14];
  X[9] = Xpts[18];  X[10] = Xpts[19]; X[11] = Xpts[20];
  shell->addStrainSVSens(strainSVSens, pt, scale, strainSens, X, vars);
}

void SolidShellWrapper::addOutputCount( int *nelems, int *nnodes, int *ncsr ){
  *nelems += 1;
  *nnodes += 4;
  *ncsr += 4;
}

void SolidShellWrapper::getOutputData( unsigned int out_type, 
                                       double *data, int ld_data,
                                       const TacsScalar Xpts[],
                                       const TacsScalar vars[] ){
  for ( int m = 0; m < 2; m++ ){
    for ( int n = 0; n < 2; n++ ){
      int node = n + 2*m;
      int index = 0;
      if (out_type & TACSElement::OUTPUT_NODES){
        for ( int k = 0; k < 3; k++ ){
          data[index+k] = TacsRealPart(Xpts[6*node+k]);
        }
        index += 3;
      }
      if (out_type & TACSElement::OUTPUT_DISPLACEMENTS){
        for ( int k = 0; k < 3; k++ ){
          data[index+k] = TacsRealPart(vars[6*node+k]);
        }
        index += 3;
      }

      data += ld_data;
    }
  }
}

void SolidShellWrapper::getOutputConnectivity( int *con, int node ){
  con[0] = node;
  con[1] = node+1;
  con[2] = node+3;
  con[3] = node+2;
}
