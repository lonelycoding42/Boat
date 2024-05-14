#ifndef PID_H
#define PID_H

class PID{
public:
	virtual float PID_Set(float Aim)=0;
protected:
  float target,actual;  //目标和实际值
	float Kp,Ki,Kd;		    //p,i,d参数
  float integral;       //积分值
	float Max,Min;		    //最大最小限幅
	float Ek,Ek1;		      //误差
};

//位置式PID
class PID_loc:public PID{
public:
	PID_loc(float p,float i,float d,float Max=179,float Min=0,float actual=75);
	float PID_Set(float Aim);
};

//增量式PID
class PID_inc:public PID{
public:
	PID_inc(float p,float i,float d,float Max=179,float Min=0,float actual=75);
	float PID_Set(float Aim);
private:
	float Ek2;
};

/************************************************************/

PID_loc::PID_loc(float p,float i,float d,float Max,float Min,float actual){
  this->Kp=p;this->Ki=i;this->Kd=d;
  this->Max=Max;this->Min=Min;
  this->actual=actual;
  integral=0;
  Ek=Ek1=0;
}
float PID_loc::PID_Set(float Aim){
  Ek=Aim-actual;
  integral+=Ek;
  actual=Kp*Ek+Ki*integral+Kd*(Ek-Ek1);
  Ek1=Ek;
  return (actual>Max?Max:actual)<Min?Min:actual;
}

PID_inc::PID_inc(float p,float i,float d,float Max,float Min,float actual){
  this->Kp=p;this->Ki=i;this->Kd=d;
  this->Max=Max;this->Min=Min;
  this->actual=actual;
  Ek=Ek1=Ek2=0;
  integral=0;
}
float PID_inc::PID_Set(float Aim){
  Ek=Aim-actual;
  float increment=Kp*(Ek-Ek1)+Ki*Ek+Kd*(Ek-2*Ek1+Ek2);
  actual+=increment;
  Ek2=Ek1;Ek1=Ek;
  return (actual>Max?Max:actual)<Min?Min:actual;
}

#endif
