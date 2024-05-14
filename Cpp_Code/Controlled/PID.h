#define PID_INTEGAL_ON

#ifndef PID_H
#define PID_H

class PID{
public:
	PID(float p,float i,float d);
	virtual float PID_Set(float Aim,float Act);
protected:
	float Kp,Ki,Kd;		//p,i,d参数
	float Max,Min;		//最大最小限幅
	float Ek,Ek1;		  //误差
};

class PID_loc:public PID{
public:
	PID_loc(float p,float i):PID(p,i,d){
		Ek=Ek1=0;
	}
#ifdef PID_INTEGAL_ON
	PID_loc(float p,float i,float d,float IntegalMax,float IntegalMin):PID(p,i,d){
		Ek=Ek1=IntegalMax=IntegalMin=0;
	}
#endif
	float PID_Set(float Aim,float Act);
private:
#ifdef PID_INTEGAL_ON
	float Integal;
	float IntegalMax,IntegalMin;
#endif
};

class PID_inc:public PID{
public:
	PID_inc(float p,float i,float d):PID(p,i,d){
		Ek=Ek1=Ek2=0;
	}
	float PID_Set(float Aim,float Act);
private:
	float Ek2;
};

#endif
