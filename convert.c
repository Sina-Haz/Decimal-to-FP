#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include<limits.h>

long bits;

void getUnsigned(double decimal,int** arr,int sizeofArr){
    //double num = floor(decimal);

    for(int i = 0;i<sizeofArr;i++){
        long binVal = 1 << (sizeofArr-i-1);
        if (binVal <= decimal){
            (*arr)[i] = 1;
            decimal = decimal - binVal;
        }
        else{
            (*arr)[i] = 0;
        }
    }

}

void getFractional(double decimal, int** arr,int sizeofArr){
    double num = decimal - floor(decimal);
    for(int i = 0;i<sizeofArr;i++){
        num = 2*num;
        (*arr)[i] = floor(num);
        if(floor(num) == 1){
            num--;
        }
    }
}


int getStartIndex(int* arr1,int* arr2,double decimal,int sizeArr1,int sizeArr2){
    int startInd = 0;
    if(floor(decimal) > 1){
        while(startInd < sizeArr1){
            startInd++;
            if(arr1[startInd - 1] == 1){
                break;
            }
        }
    }
    else if(floor(decimal) == 1){
        startInd = sizeArr1;
    }
    else if(floor(decimal) == 0){
        startInd = startInd+sizeArr1;
        while((startInd-sizeArr1) < sizeArr2){
            startInd++;
            if(arr2[startInd - 1 - sizeArr1] == 1){
                break;
            }
        }
    }
    return startInd;
}


int MakeCanonicalArr(int* arr1,int* arr2,int** M,int startInd,int sizeArr1,int sizeArr2){
    int exp;
    exp = sizeArr1 - startInd;

    if(exp == 0){
        for(int i = 0;i < bits;i++){
            (*M)[i] = arr2[i];
        }
    }
    else if(exp > 0){
        int count = 0;
        //this for loop copies over arr1 while still space in M
        for(int i = startInd;i<sizeArr1;i++){
            if(count < bits){
                (*M)[count] = arr1[i];
                count++;
            }
        }
        for(int i = 0;i < sizeArr2;i++){
            if(count < bits){
                (*M)[count] = arr2[i];
                count++;
            }
        }
    }
    else{
        int count = 0;
        for(int i = startInd - sizeArr1;i < sizeArr2;i++){
            if(count < bits){
                (*M)[count] = arr2[i];
                count++;
            }
        }
        if(count < bits){
            while(count < bits){
                (*M)[count] = 0;
                count++;
            }
        }
    }
    return exp;
}

double convToDouble(int** M,int exp,int isNormal,int sizeofM){
    double multiplier = pow(2.0,1.0*exp);
    double num = 0;
    if(isNormal){num+=multiplier;}
    for(int i = 0;i < sizeofM;i++){
        multiplier /= 2;
        num += (*M)[i] * multiplier;
    }
    return num;
}

int roundUp(int** M,int sizeofM){
    for(int i = sizeofM-1;i >= 0;i--){
        if((*M)[i] == 1){
            (*M)[i] = 0;
            if(i == 0){
                return 1;
            }
        }
        else if((*M)[i] == 0){
            (*M)[i] = 1;
            break;
        }
    }
    return 0;
}


int roundM(int** M,int sizeofM, int exp, double decimal,int isNormal){
    int expAdd = 0;
    int* midpt = malloc(sizeof(int)*(sizeofM+1));
    for(int i = 0;i<sizeofM;i++){
        midpt[i] = (*M)[i];
    }
    midpt[sizeofM] = 1;
    if(decimal > convToDouble(&midpt,exp,isNormal,sizeofM+1)){
        expAdd += roundUp(M,sizeofM);
    }
    else if(decimal == convToDouble(&midpt,exp,isNormal,sizeofM+1) && (*M)[sizeofM-1] == 1){
        expAdd += roundUp(M,sizeofM);
    }
    free(midpt);
    return expAdd;
}

void fillFP(int** FParr,int* M,int* E, int Mbits,int Ebits){
    int i;
    for(i = 0;i<Ebits;i++){
        (*FParr)[i+1] = E[i];
    }
    for(i = 0;i < Mbits;i++){
        (*FParr)[i+1+Ebits] = M[i];
    }
}

void getSubnormalM(int** M,int exp,int sizeofM,double decimal){
    double multiplier = pow(2.0,1.0*exp);
    for(int i = 0;i < sizeofM;i++){
        multiplier = multiplier/2;
        if(multiplier <= decimal){
            decimal -= multiplier;
            (*M)[i] = 1;
        }
    }
}

void printBinary(int* arr,int arrSize){
    for(int i = 0;i < arrSize;i++){
        printf("%d",arr[i]);
    }
    printf("\n");
}

int main(int argc,char* argv[]){
    if (argc > 1){
        double decimal;
        long totalBits;
        long Ebits;
        FILE* fp = fopen(argv[1],"r");
        while(fscanf(fp,"%lf %ld %ld %ld\n",&decimal,&totalBits,&Ebits,&bits) != EOF){
            int* FParr = malloc(sizeof(int)*totalBits);
            if(decimal < 0){
                FParr[0] = 1;
                decimal = -decimal;
            }
            else{
                FParr[0] = 0;
            }
            int Eval;
            int* M = malloc(sizeof(int)*bits);
            int* E = malloc(sizeof(int)*Ebits);
            long bias = (1 << (Ebits-1)) - 1;
            double maxNormalRep = pow(2.0,1+bias);
            double minNormalRep = pow(2.0,(1.0 - bias));
            if (decimal >= minNormalRep && decimal < maxNormalRep){
                int sizeArr1 = 1+bias;
                int sizeArr2 = bias;
                if(maxNormalRep > INT_MAX || sizeArr1 < bits || sizeArr2 < bits){
                    sizeArr1 = bits;
                    sizeArr2 = bits;
                }
                int* arr1 = malloc(sizeof(int)*(sizeArr1));
                getUnsigned(decimal,&arr1,sizeArr1);
                int* arr2 = malloc(sizeof(int)*(sizeArr2));
                getFractional(decimal,&arr2,sizeArr2);
                Eval = MakeCanonicalArr(arr1,arr2,&M,getStartIndex(arr1,arr2,decimal,sizeArr1,sizeArr2),sizeArr1,sizeArr2) + bias;
                Eval += roundM(&M,bits,Eval-bias,decimal,1);
                getUnsigned(Eval*1.0,&E,Ebits);
                fillFP(&FParr,M,E,bits,Ebits);
                printBinary(FParr,totalBits);
                free(arr1);
                free(arr2);
            }
            else{
                for(int i = 0;i < Ebits;i++){
                    E[i] = 0;
                }
                getSubnormalM(&M,1-bias,bits,decimal);
                E[Ebits-1] = roundM(&M,bits,1-bias,decimal,0);
                fillFP(&FParr,M,E,bits,Ebits);
                printBinary(FParr,totalBits);
            }
            free(M);
            free(E);
            free(FParr);
        }
    }

    return EXIT_SUCCESS;
}