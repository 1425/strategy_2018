#include<Python.h>
#include "util.h"

//the point of this file is to calculate the values of the Skellam distribution by calling out to Python's scipy.

double skellam_cdf(double m1,double m2,double x){
	nyi
	return m1+m2+x;
}

int main1(int /*argc*/,char **argv){
	wchar_t *program=Py_DecodeLocale(argv[0],NULL);
	if(program==NULL){
		fprintf(stderr,"Fatal error: cannot decode argv[0]\n");
		exit(1);
	}
	Py_SetProgramName(program);
	Py_Initialize();
	PyRun_SimpleString("from time import time,ctime\n"
		"print('Today is',ctime(time()))\n");
	if(Py_FinalizeEx()<0){
		exit(120);
	}
	PyMem_RawFree(program);
	return 0;
}

int main2(int argc,char **argv){
	PyObject *pName,*pModule,/* *pDict,*/ *pFunc;
	PyObject *pArgs,*pValue;
	int i;
	if(argc<3){
		fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
		return 1;
	}

	Py_Initialize();
	pName=PyUnicode_DecodeFSDefault(argv[1]);

	pModule=PyImport_Import(pName);
	Py_DECREF(pName);

	if(pModule!=NULL){
		pFunc=PyObject_GetAttrString(pModule,argv[2]);
		/*pFunc is a new reference*/

		if(pFunc&&PyCallable_Check(pFunc)){
			pArgs=PyTuple_New(argc-3);
			for(i=0;i<argc-3;++i){
				pValue=PyLong_FromLong(atoi(argv[i+3]));
				if(!pValue){
					Py_DECREF(pArgs);
					Py_DECREF(pModule);
					fprintf(stderr,"Cannot convert argument\n");
					return 1;
				}
				/*pValue reference stolen here:*/
				PyTuple_SetItem(pArgs,i,pValue);
			}
			pValue=PyObject_CallObject(pFunc,pArgs);
			Py_DECREF(pArgs);
			if(pValue!=NULL){
				printf("Result of call: %ld\n",PyLong_AsLong(pValue));
				Py_DECREF(pValue);
			}else{
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				fprintf(stderr,"Call failed\n");
				return 1;
			}
		}else{
			if(PyErr_Occurred()){
				PyErr_Print();
			}
			fprintf(stderr,"Cannot find function \"%s\"\n",argv[2]);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	}else{
		PyErr_Print();
		fprintf(stderr,"Failed to load \"%s\"\n",argv[1]);
		return 1;
	}

	//to try to get rid of issue with unloading threading module
	//Py_DECREF(PyImport_ImportModule("threading"));

	if(Py_FinalizeEx()<0){
		return 120;
	}
	return 0;
}

struct Subprocess{
	
};

int main(){
	//Subprocess x;
	nyi//x.run("what?");
}
