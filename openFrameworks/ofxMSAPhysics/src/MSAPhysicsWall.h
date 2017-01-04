#pragma once

#include "MSAPhysics.h"

namespace msa {
	
  namespace physics {
		
    template <typename T>
      class WallT  {
    public:
      
      WallT(T P1e, T P2e,T P3e);
      ~WallT();

      void set(T P1e, T P2e,T P3e);

      float distance(T v);
      T projOnNorm(T v);
      T projOnSurf(T v);
      T reflect(T v);
      int sameSide(T P,T A,T B,T C);
      int isOnSurf(T P);
      int isOnSurf1(T P);
      int isOnSurf2(T P);
      int impact(ParticleT<T> *a);
      
      const T& getVertex(int i);

      T P1, P2, P3, P4;
      T Dir21, Dir31;
      T Norm;
      T InvNorm;
      bool IsInf[3];
      float dPar;
    };

    //--------------------------------------------------------------
    template <typename T>
      WallT<T>::WallT(T P1e, T P2e,T P3e) {
      set(P1e,P2e,P3e);
    }

    //--------------------------------------------------------------
    template <typename T>
      WallT<T>::~WallT() {
    }
    //--------------------------------------------------------------
    template <typename T>
      void WallT<T>::set(T P1e, T P2e,T P3e) {
      P1 = P1e;
      P2 = P2e;
      P3 = P3e;
      Dir21 = P2 - P1;
      Dir31 = P3 - P1;
      P4 = P1 + Dir21 + Dir31;
      /* Norm = Dir1 ^ Dir2; */
      Norm.x = Dir21.y*Dir31.z - Dir21.z*Dir31.y;
      Norm.y = Dir21.z*Dir31.x - Dir21.x*Dir31.z;
      Norm.z = Dir21.x*Dir31.y - Dir21.y*Dir31.x;
      Norm.normalize();
      for(int d=0;d<3;d++){
	if( fabs(Norm[d]) > 0.){
	  InvNorm[d] = 1./Norm[d];
	  IsInf[d] = 0;
	}
	else{
	  InvNorm[d] = 0.;
	  IsInf[d] = 1;
	}
      }
      dPar = - P1.x*Norm.x - P1.y*Norm.y - P1.z*Norm.z;
    }
    //--------------------------------------------------------------
    template <typename T>
      float WallT<T>::distance(T Pos) {
      float Dist = fabs(Norm.x*Pos.x + Norm.y*Pos.y + Norm.z*Pos.z + dPar);
      return Dist;
    }
    //--------------------------------------------------------------
    template <typename T>
      T WallT<T>::projOnSurf(T Pos) {
      T Pos1 = Pos;
      float Dist = distance(Pos);
      float Sig = 0.;
      for(int d=0;d<3;d++){
	Sig += (P1[d] - Pos[d])*Norm[d];
      }
      if(Sig > 0) Sig = -1.;
      else Sig = 1.;
      for(int d=0;d<3;d++){
	Pos1[d] = Pos[d] - Sig*Dist*Norm[d];
      }
      return Pos1;
    }		
    //--------------------------------------------------------------
    template <typename T>
      T WallT<T>::projOnNorm(T v) {
      float Len = 0.;
      for(int d=0;d<3;d++){
	Len += v[d]*Norm[d];
      }
      T Proj(Len*Norm[0],Len*Norm[1],Len*Norm[2]);
      return Proj;
    }
    //--------------------------------------------------------------
    template <typename T>
      T WallT<T>::reflect(T v) {
      T n1 = projOnNorm(v);
      T v1 = v - 2.*n1;
      return v1;
    }
    //--------------------------------------------------------------
    template <typename T>
      int WallT<T>::sameSide(T P,T A,T B,T C) {
      /* T D1 = (C-B)^(P-B); */
      /* T D2 = (C-B)^(A-B); */
      T D1;
      T D2;
      double Scal = 0.;
      for(int d=0;d<3;d++){
	int NUno = (d+1)%3;
	int NDue = (d+2)%3;
	D1[d] = (C[NUno]-B[NUno])*(P[NDue]-B[NDue]) - (C[NDue]-B[NDue])*(P[NUno]-B[NUno]);
	D2[d] = (C[NUno]-B[NUno])*(A[NDue]-B[NDue]) - (C[NDue]-B[NDue])*(A[NUno]-B[NUno]);
	Scal += D1[d]*D2[d];
      }
      if( Scal >= 0 ) return 1;
      return 0;
    }
    //--------------------------------------------------------------
    template <typename T>
      int WallT<T>::isOnSurf(T P){
      return isOnSurf2(P);
    }
    //--------------------------------------------------------------
    template <typename T>
      int WallT<T>::isOnSurf1(T P){
      if( sameSide(P,P1,P2,P3) && sameSide(P,P2,P1,P3) && sameSide(P,P3,P1,P2) ) return 1;
      return 0;
    }
    //--------------------------------------------------------------
    template <typename T>
      int WallT<T>::isOnSurf2(T P){
      T DirP1 = P - P1;
      double sq2 = 0.,sq3 = 0.;
      double pd32 = 0., pdP3 = 0., pdP2 = 0.;
      for(int d=0;d<3;d++){
	sq2 += Dir21[d]*Dir21[d];
	sq3 += Dir31[d]*Dir31[d];
	pd32 += Dir21[d]*Dir31[d];
	pdP3 += Dir31[d]*DirP1[d];
	pdP2 += Dir21[d]*DirP1[d];
      }
      double Inv = 1./(sq2*sq3 - pd32*pd32);
      double v = (sq3*pdP2 - pd32*pdP3)*Inv;
      double u = (sq2*pdP3 - pd32*pdP2)*Inv;
      if(u >= 0. && v >= 0. && (u+v) < 1.) return 1;
      else return 0;
    }
    //--------------------------------------------------------------
    template <typename T>
      int WallT<T>::impact(ParticleT<T> *a) {
      T Pos = a->getPosition();
      float Rad = 2.*a->getRadius();
      T ProjP(Norm.x*(Pos.x - P1.x),Norm.y*(Pos.y - P1.y),Norm.z*(Pos.z - P1.z));
      float PreP = ProjP.x + ProjP.y + ProjP.z;
      if( fabs(PreP) > Rad ) return 0;
      float Sign = PreP > 0. ? -1. : 1.;
      T PosS = projOnSurf(Pos);
      if(!isOnSurf(PosS)) return 0;
      T Vel = a->getVelocity();
      /* T Vel1 = a->getBounce()*reflect(Vel); */
      T Vel1 = reflect(Vel);
      T Pos1 = PosS - Sign*Rad*Norm;
      /* /\* T Vel1 = Vel; *\/ */
      /* /\* T Pos1 = Pos; *\/ */
      /* for(int d=0;d<3;d++){ */
      /*   if(fabs(ProjP[d]) < fabs(Rad*InvNorm[d])){ */
      /*     /\* Pos1[d] = PosS[d] - Sign*Rad*Norm[d]; *\/ */
      /*     /\* Vel1[d] = -Vel[d]*Norm[d]*a->getBounce(); *\/ */
      /*   } */
      /* } */
      a->setVelocity(Vel1);
      a->moveTo(Pos1,true);
      return 1;
    }
    //--------------------------------------------------------------
    template <typename T>
      inline const T& WallT<T>::getVertex(int i) {
      if(i == 1) return P1;
      if(i == 2) return P2;
      if(i == 3) return P3;
      if(i == 4) return P4;
      return P1;
    }
		
  }
}

