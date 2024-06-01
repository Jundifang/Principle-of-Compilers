/*
---------------------------------------------

    单词类型    名称            符号
    1           标识符
    2           关键字         int、float、string、if、else、while、return、break、and、or
    3           字符串
    4           整数
    5           小数
    6           分隔符          {、 }、 (、 )、 ；、,
    7           四则运算符      +、-、*、/ 
    8           比较运算符      ==、<、<=、>、>=、!=
    9           赋值运算符      =
    10          逻辑运算符      && ||
    11          复合运算符      += -= *= /=

---------------------------------------------
*/
/*词法分析器 / wordsAnalysis*/
#include <bits/stdc++.h>
#include<fstream>
using namespace std;

typedef struct{
    int row,column;
    char c;
}unit;

stack<unit> twin;//用于成对符号判断
int row=1,k;
#define col k+1        //仅用于输出位置
int tagflag=0;//tagflag用于注释
int stringflag=0;//stringflag用于字符串

string str;
string tmp;

//报错(row,col,文字报错类型)
void error(int i,int j,string errortype="未知"){
    cout<<endl<<"Fault: in row "<<i<<" column "<<j<<" 报错原因: "<<errortype<<"\n"; 
}

//打印二元式(开始,结束,类型)
void print(int start,int end,int type){
    cout<<'('<<type<<','<<str.substr(start,end-start+1)<<','<<row<<','<<start+1<<')'<<endl;
}

//数字的可连接字符
int checknum(char c){
    if(c==';' || c==','|| c=='<'|| c=='>' || c=='='|| c=='!'
    || c==')'|| c=='+'|| c=='-'|| c=='*'|| c=='/'||c==' ')
        return 1;
    else return 0;
}

//标识符的可连接字符
int checkid_otherchar(char c){
    if(c==';' || c==','|| c=='<'|| c=='>' || c=='='|| c=='!'
    || c==')'|| c=='+'|| c=='-'|| c=='*'|| c=='/'||c=='(')
        return 1;
    else return 0;
}
//整数取舍  
int Comparedigit(string s){
    string maxn="65536";
    int a=maxn.length(),b=s.length();
    if(b<a) return 1;
    else if(b==a && s<maxn) return 1;
    else return 0;
}
int main()
{
    
    ifstream file("demo/demoG.txt",ios::in);
    if(!file){
        cout<<"Fail to open. please try again!";
        exit(-1);
    }
    else{
        cout<<"success!\n\n";

        while(getline(file,str)){
            int len=str.length();
            k=0;
            
            //一行内解决
            while(k<len){
                //处理 */
                if(tagflag){
                    for(int i=k;i<len-1;++i){
                        if(str[i]=='*' && str[i+1]=='/'){
                            tagflag=0;k=i+2;
                            if(twin.empty()){
                                cout<<"*/处理出错，请检查！";
                            }
                            else if(twin.top().c=='/')//弹出/*
                                twin.pop();
                            else    cout<<"*/处理出错，请检查！";
                            break;
                        }
                    }
                    if(tagflag)break;
                    continue;
                }
                
                //处理字符串    右"     
                if(stringflag){
                    unit a;
                    for(int i=k;i<len;++i){
                        if(str[i]=='"'){
                            stringflag=0;k=i+1;
                            //弹出/*
                            if(twin.empty()){
                                cout<<"'\"'处理出错，请检查！";
                            }
                            else if(twin.top().c=='"'){
                                a=twin.top();
                                twin.pop();
                            }
                                
                            else    cout<<"字符串右'\"'处理出错，请检查！";
                            break;
                        }
                    }
                    if(stringflag){
                        tmp+=str;
                        break;
                    }
                    else{//字符串结束，需要输出
                        
                        tmp+=str.substr(0,k);
                        cout<<'('<<3<<','<<tmp<<','<<a.row<<','<<a.column<<')'<<endl;
                        tmp="";
                    }
                    continue;
                }

                if(str[k]==' ' || str[k]=='\n' || str[k]=='\t'){k++;continue;}
                //. 属于数字报错
                else if(str[k]=='.' && k+1<len && isdigit(str[k+1])){
                    error(row,col,"小数点前缺失数字");
                    while(isdigit(str[++k]));
                    continue;
                }

                //   ,   ; type: 6
                else if(str[k]==';' || str[k]==','){
                    print(k,k,6);k+=1;continue;
                }
                
                //   ！= type: 8
                else if(k+1<len && str[k]=='!' && str[k+1]=='='){
                    print(k,k+1,8);k+=2;continue;
                }

                // > < >= <=
                else if(str[k]=='<' || str[k]=='>'){
                    if(k+1<len){
                        if(str[k+1]=='='){print(k,k+1,8);k+=2;continue;}// <= >= type:8
                        else {print(k,k,8);k+=1;continue;}// > < type:8
                    }
                    else {print(k,k,8);k+=1;continue;}// > < type:8
                }

                // * *= 
                else if(str[k]=='*'){ 
                    if(k+1<len){
                        // *= type:11
                        if(str[k+1]=='='){print(k,k+1,11);k+=2;continue;}
                        else {print(k,k,7);k+=1;continue;}   // * type:7
                        
                        
                    }
                    else {print(k,k,7);k+=1;continue;}// * type:7
                }

                // = ==
                else if(str[k]=='='){
                    if(k+1<len){
                        if(str[k+1]=='='){print(k,k+1,8);k+=2;continue;}// == type:8
                        else {print(k,k,9);k+=1;continue;}   // = type:9
                    }
                    else {print(k,k,9);k+=1;continue;}// = type:9
                }

                // + - += -=
                else if(str[k]=='+' || str[k]=='-'){
                    if(k+1<len){
                        if(str[k+1]=='='){print(k,k+1,11);k+=2;continue;}// += -= type:11
                        else {print(k,k,7);k+=1;continue;}// + - type:7
                    }
                    else {print(k,k,7);k+=1;continue;}// + - type:7
                }

                //&& ||
                else if(str[k]=='&' || str[k]=='|'){
                    if(k+1<len){
                        if(str[k+1]==str[k]){print(k,k+1,10);k+=2;continue;}// && || type:10
                        else {error(row,col,string(1,str[k])+"不成对");k+=1;continue;}// 
                    }
                    else {error(row,col,string(1,str[k])+"不成对");k+=1;continue;}// & |报错
                }
                
                // { ( type: 6        
                else if(str[k]=='{' || str[k]=='(' ){
                    unit* sign=new unit{row,col,str[k]};
                    twin.push(*sign);
                    print(k,k,6);k+=1;continue;
                }

                //字符串开始 左"
                else if(str[k]=='"'){
                    unit* sign=new unit{row,col,str[k]};
                    twin.push(*sign);
                    stringflag=1;
                    int start=k++;
                    //处理同行右半个"
                    for(;k<len;++k){
                        if(str[k]=='"'){
                            unit a=twin.top();
                            if(a.c=='"'){
                                twin.pop();
                            }
                                
                            else    cout<<"字符串右半边处理出错，请检查！";
                    
                            stringflag=0;
                            cout<<'('<<3<<','<<str.substr(start,k+1-start)<<','<<a.row<<','<<a.column<<')'<<endl;
                            k++;
                            break;
                        }
                    }
                    if(k>=len){
                        tmp=str.substr(start);     break;
                    }
                    continue;
                }
                //  // /* / /=
                else if(str[k]=='/'){
                    if(k+1<len){
                        if(str[k+1]=='/')break;//     //注释
                        else if(str[k+1]=='*'){//       /*注释
                            tagflag=1;
                            unit* sign=new unit{row,col,str[k]};//只存入/代替/*
                            twin.push(*sign);
                            k+=2;continue;
                            
                        }
                        else if(str[k+1]=='='){print(k,k+1,11);k+=2;continue;}//        /= type: 11
                        else    {print(k,k,7);k+=1;continue;}//    / type: 7
                    }
                    else    {print(k,k,7);k+=1;continue;}  //       / type: 7

                }

                // } ) type: 6     已使用stack      
                else if(str[k]==')' || str[k]=='}'){
                    switch(str[k]){
                        case ')':
                            if(twin.empty()){
                                cout<<")处理出错，请检查！";
                            }
                            else if(twin.top().c=='('){
                                twin.pop();
                            }
                            else error(twin.top().row,twin.top().column,"缺少)");
                            break;
                        case '}':
                            if(twin.empty()){
                                cout<<"}处理出错，请检查！";
                            }
                            else if(twin.top().c=='{'){
                                twin.pop();
                            }
                            else error(twin.top().row,twin.top().column,"缺少与'{'匹配的'}'");
                            break;
                    }
                    print(k,k,6);k+=1;continue;
                }

                //整数 4  无0开头
                else if(isdigit(str[k])){
                    //判断0 以及 0d(报错)
                    if(str[k]=='0'){
                        if(k+1<len){
                            int start=k;
                            if(isdigit(str[k+1])){error(row,col,"出现0开头的整数");while(isdigit(str[++k]));continue;}
                            else if(checknum(str[k+1])){print(k,k,4);k++;} //0
                            //digit+alpha 非法标识符
                            else if(isalpha(str[k+1])){
                                error(row,col,"非法: 标识符以数字开头");
                                while(++k<len && (isalpha(str[k]) || isdigit(str[k]) || str[k]=='_'));
                                //输出非法标识符
                                print(start,k-1,4);continue;
                            }
                            //小数
                            else if(str[k+1]=='.'){
                                int st1=++k;
                                while(isdigit(str[++k])&& k<len);
                                if(k==st1 && k<len){error(row,col,"小数点后缺少数字");continue;}
                                else if(k-st1<=7)     print(start,k-1,5);
                                else print(start,st1+6,5);
                                continue;
                            }
                            else if(str[k+1]==' '){print(k,k,4);k+=2;continue;}
                            else    {error(row,col,"非法字符与数字连接: "+string(1,str[k]));++k;continue;}
                        }
                        //读取完毕
                        else {print(k,k,4);break;}//0;
                        continue;
                    }
                    //非0开头
                    
                    if(k+1<len){
                        int start=k;
                        while(isdigit(str[++k])&& k<len );
                        //小数 type:5
                        if(str[k]=='.'){
                            int st1=k;
                            while(isdigit(str[++k])&& k<len);
                            if(k==st1+1 && k<len){error(row,col,"小数点后缺少数字");continue;}
                            else if(k-st1<=7)     print(start,k-1,5);
                            else print(start,st1+6,5);
                            continue;
                        }
                        //digit+alpha 非法标识符
                        else if(isalpha(str[k])){
                            error(row,col,"非法: 标识符以数字开头");
                            while(++k<len && (isalpha(str[k]) || isdigit(str[k]) || str[k]=='_'));
                            //输出非法标识符
                            print(start,k-1,4);continue;
                        }

                        //正常数字结束
                        else if(checknum(str[k])){
                            if(Comparedigit(str.substr(start,k-start)))print(start,k-1,4);
                            else error(row,col,"整数: "+str.substr(start,k-start)+"超过16bits\n");
                            continue;
                        }
                        else if(str[k]==' '){print(start,k-1,4);k+=1;continue;}
                        else    {error(row,col,"非法字符与数字连接: "+string(1,str[k]));++k;continue;}
                    }
                    //读取完毕      
                    else print(k,k,4);
                    continue;
                }
                // _打头的id
                else if(str[k]=='_'){
                    int sumdig=0;//id中数字个数
                    int start=k++;
                    int flag=1;
                    while(k<len){
                        while(isalpha(str[k]) || str[k]=='_' || isdigit(str[k])){
                            if(sumdig>9){//超数字个数
                                error(row,col,"数字超过10个");
                                cout<<'('<<1<<','<<str.substr(start,k-start)<<','<<row<<','<<start+1<<')'<<endl;
                                flag=0;
                            }
                            if(k-start>32){
                                error(row,col,"字符总数超过32个");
                                cout<<'('<<1<<','<<str.substr(start,32)<<','<<row<<','<<start+1<<')'<<endl;
                                flag=0;
                            }
                            if(!flag)break;
                            if(isdigit(str[k]))++sumdig;
                            ++k;
                        }
                        //忽略后续所有字符串
                        while(isalpha(str[k]) || str[k]=='_' || isdigit(str[k]))k++;
                        //数字不超10 且 总长不超32的输出
                        if(flag){
                            if(checkid_otherchar(str[k])){
                                print(start,k-1,1);
                                break;
                            }
                            else if(str[k]==' '){print(start,k-1,1);break;}
                            else {error(row,col,"出现非法标识符字符: "+string(1,str[k]));print(start,k-1,1);break;}//非_/字母/数字的报错退出id
                        }
                        break;

                    }
                    continue;
                }
                // id 1 或者 保留字 2
                else if(isalpha(str[k])){
                    //判断保留字
                    string x=str.substr(k,2);//if   or
                    if(x=="if" || x=="or")  {cout<<'('<<2<<','<<x<<','<<row<<','<<col<<')'<<endl;k+=2;continue;}
                    else{
                        x=str.substr(k,3);//int and
                        if(x=="int" ||x=="and"){cout<<'('<<2<<','<<x<<','<<row<<','<<col<<')'<<endl;k+=3;continue;}
                        else{
                            x=str.substr(k,4);//else    void
                            if(x=="else" || x=="void"){cout<<'('<<2<<','<<x<<','<<row<<','<<col<<')'<<endl;k+=4;continue;}
                            else{
                                x=str.substr(k,5);//float   while   break
                                if(x=="float" ||x=="while" || x=="break" ){cout<<'('<<2<<','<<x<<','<<row<<','<<col<<')'<<endl;k+=5;continue;}
                                else{
                                    x=str.substr(k,6);//string  return
                                    if(x=="string" ||x=="return"){cout<<'('<<2<<','<<x<<','<<row<<','<<col<<')'<<endl;k+=6;continue;}
                                    
                                    //id处理
                                    else{
                                        int sumdig=0;//id中数字个数
                                        int start=k++,flag=1;
                                        if(k<len)
                                        while(k<len){
                                            while(isalpha(str[k]) || str[k]=='_' || isdigit(str[k])){
                                                if(sumdig>9){//超数字个数
                                                    error(row,col,"数字超过10个");
                                                    cout<<'('<<1<<','<<str.substr(start,k-start)<<','<<row<<','<<start+1<<')'<<endl;
                                                    flag=0;
                                                }
                                                if(k-start>32){
                                                    error(row,col,"字符总数超过32个");
                                                    cout<<'('<<1<<','<<str.substr(start,32)<<','<<row<<','<<start+1<<')'<<endl;
                                                    flag=0;
                                                }
                                                if(!flag)break;
                                                if(isdigit(str[k]))++sumdig;
                                                ++k;
                                            }
                                            //忽略后续所有字符串
                                            while(isalpha(str[k]) || str[k]=='_' || isdigit(str[k]))k++;
                                            //数字不超10 且 总长不超32的输出
                                            if(flag){
                                                if(checkid_otherchar(str[k])){
                                                    print(start,k-1,1);
                                                    break;
                                                }
                                                else if(str[k]==' '){print(start,k-1,1);break;}
                                                else {error(row,col,"出现非法标识符字符: "+string(1,str[k]));print(start,k-1,1);break;}//非_/字母/数字的报错退出id
                                            }
                                            break;
                                        }
                                        else {cout<<'('<<1<<','<<str[start]<<','<<row<<','<<start+1<<')'<<endl;}
                                    }
                                }
                            }
                        }
                    }
                    continue;
                }
                //否则出错
                else {error(row,col,"Unknown words: "+string(1,str[k]));k++;}
            }
            row++;
        }
        while(!twin.empty()){
            if(twin.top().c=='/'){
                
                error(twin.top().row,twin.top().column,"缺少块注释*/");
                twin.pop();
            }
            else if(twin.top().c=='"'){
                error(twin.top().row,twin.top().column,"缺少字符串右半边双引号");
                twin.pop();

            }
            else if(twin.top().c=='('){
                error(twin.top().row,twin.top().column,"缺少)");
                twin.pop();

            }
            else if(twin.top().c=='{'){
                error(twin.top().row,twin.top().column,"缺少}");
                twin.pop();

            }
        }
            
        cout<<"\n\n*********输出完毕*********\n\n";
        file.close();
    }

    return 0;
}
