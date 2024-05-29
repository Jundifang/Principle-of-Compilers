/*---------------------------------------------

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

---------------------------------------------*/
/*语法分析器*/
#include <bits/stdc++.h>
#include<fstream>
using namespace std;

//结构体定义
//词法分析器的word
typedef struct word{
    int   type;
    string  id;
    int   row;
    int   col;
}token;
//括号的匹配
typedef struct{
    int row,column;
    string c;
}unit;
/*
//break跳转{
typedef struct{
    int line;
    string c;
}jpkh;
*/
//四元组
typedef struct out{
    string   sign,a,b,pos;
}quat;

//函数声明
bool isfuncredef(const token &p);
bool ispararedef(const token &p);
inline void backreturn();
inline void backbreak();
void fullsen();
void xunhuansen();
void ifsen();
void partialsen();
void manip();
void fuzhi();
void bijiao();
void sizeyunsuan();
void dingA();
void dingyi();
void dingB();
void bijiaoA();
void dingC();
bool dy();
bool bsf();
bool tj();
bool lj();
bool xh();
bool rt();
bool zfc();
bool zs();
bool xs();
bool dkh();
bool xkh();
bool fh();
bool dh();
bool szysf();
bool bjysf();
bool fzysf();
int isdiffprejunc();



ifstream file("output_words.txt",ios::in);

stack<string>ifstack,emptystack;//if的顺序判断
queue<token>prelist;//预读队列
stack<unit> twin;//{} ()判断
set<string>paraset,newparaset,last;//变量set
set<string>funcset;//函数set
map<string,int>paramap;//变量map，<变量名，使用状态>
//变量定义栈，新一个{出现则压入旧set，出一个}弹出一个set
stack<set<string> >parastack;
map<string,int>funcmap;//定义的函数map，<函数名，参数个数>

string str;//读取词法分析器
string a,b;//四则运算时存储操作数
string szid;//四则运算符的首位操作数
string fzid;//赋值的左操作数
string boolid;//保留
string bjid;//比较的首位操作数,如x&&y的x
string funcname;//函数名
string prejunc;//前一个逻辑连接
int tc=0,fc=0;//tc是true值的链，fc是false值的链
int sennum=1;//语句个数
int andfg,orfg;//and连接或者or连接
int szbjfg=1;//一组比较运算出现四则运算
int judgepdfuzhi=0;//1为已有赋值，0为未有
int funccall=0;//0为定义，1为函数调用
int dyhanshu=0;//0为变量定义，1为函数定义
int ifwhile=0;//0为其他，1为判断语句
bool flag=true;//语法分析器结果
bool ifhwhilefg;//true则缺少完整大括号
int hanshuparanum=0;//函数定义参数个数
int usehanshuparanum=0;//函数调用参数个数
int Tnum=0;//运算的Tn
int dyfg=0;//是否在定义
int breakpos;//break所在line，用于回填
int returnpos;//return所在line，用于回填
int presize;//原先twin栈多少size，若函数体结束定义则大小为presize
int dyhanshuti;//定义函数体为1
int iforwhile;//if 0 or while 1
bool isbreakexists=false;//前面是否出现break，若有1，否则0
bool isreturnexists=false;//前面是否出现return，若有1，否则0


unit sign=*(new unit);
token curw=*(new token{0," ",0,0});
vector<quat> quatlist(1,{"0","0","0","0"}); //四元组存储
// vector<int> funcparalist,newfuncparalist;//函数参数表的列表

// stringstream ss;

//保留字
string dingyigjz[3]={"int","float","string"};
string tiaojian[2]={"if","else"};
string luoji[2]={"or","and"};
string xunhuan[2]={"while","break"};
string ret="return";

//有则读，没则type 0，优先读取prelist
void read(){
    if(file.peek()!=EOF){
        char c;
        file>>c;
        while(c=='\n')file>>c;
        if(c==0)return;
        
        //词法分析已有的错误
        while(c!='('){
            while(c=='\n')file>>c;
            getline(file,str);
            cout<<'\n'<<c<<str<<"\t------ from wordsAnalysis\n";
            flag=false;
            if(file.peek()!=EOF){file>>c;continue;}
            else break;

        }
        if(prelist.empty()){
            file>>curw.type>>c;
            getline(file,curw.id,',');
            if(curw.id.empty()){curw.id=",";file>>c;}
            file>>curw.row
            >>c>>
            curw.col>>c;
        }
        else {
            curw=prelist.front();
            prelist.pop();
        }
    }
    else return;
}

//最后输出四元组
void printall(){
    for(int i=1;i<quatlist.size();++i)
        cout<<i<<": ("<<quatlist[i].sign<<" ,"<<quatlist[i].a<<" ,"<<quatlist[i].b<<" ,"<<quatlist[i].pos<<" )\n";
}

// 存入quat结构体数组(跳转符号sign/运算符 ,2 ,3 , 位置/操作数),结束后为下一个line的sennum
void putquat(string sign,string a,string b,string pos="0"){
    quatlist.push_back({sign,a,b,pos});
    ++sennum;
}

//报错(row,col,文字报错类型) 此刻curw为;下一位
void error(int i,int j,string errortype="未知"){
    flag=false;
    cout<<endl<<"Fault: in row "<<i<<" column "<<j<<" 报错原因: "<<errortype<<"\t------ 忽略后续至下一个语句 <从 ";
    while(curw.id!=";" && curw.id!="{" &&curw.id!="(" &&curw.id!=")"&&curw.id!="}"){
        read();
    }
    cout<<curw.id<<" 开始>\n";
}

//回填t和f,k代表偏移误差，0为提前back，1为默认back
void back(string sign,int k=1){
    string tmp="";
    if(sign=="t"){
        while(tc){
            tmp=quatlist[tc].pos;
            quatlist[tc].pos=to_string(sennum-k);
            tc=stoi(tmp);
        }
    }
    else if(sign=="f"){
        while(fc){
            tmp=quatlist[fc].pos;
            quatlist[fc].pos=to_string(sennum-k);
            fc=stoi(tmp);
        }
    }
}
//判断变量是否重复定义
bool ispararedef(const token &p=curw){
    for(auto &i:paraset){
        if(i==p.id)return true;
    }
    return false;
}

//判断函数是否已定义
bool isfuncredef(const token &p=curw){
    for(auto &i:funcset){
        if(i==p.id)return true;
    }
    return false;
}

//判断变量是否已定义
void isparadef(const token &p=curw){
    if(p.type!=1)return;
    int fg=0;
    for(auto &i : paraset) {
        if(i==p.id){fg=1;break;}
    }
    if(fg==0){
        for(auto &i : parastack.top()) {
            if(i==p.id){fg=1;break;}
        }
    }
    if(fg){
        if(dyfg && dyhanshu==0 && !ispararedef()){
            paramap[p.id]=1;//是否使用指标
            cout<<"变量"<<p.id<<"重复定义\n";
        }
        else{
            paramap[p.id]=1;//是否使用指标
            cout<<"变量"<<p.id<<"已定义\n";
        }
        return;
    }
    else {
        cout<<"变量 "<<p.id<<" 未定义\n";
    }
}

//判断函数是否已定义
void isfuncdef(const token &p=curw){
    if(p.type!=1)return;
    int fg=0;
    for(auto &i : funcset) {
        if(i==p.id){fg=1;break;}
    }
    if(fg){
        if(dyfg && dyhanshu &&!isfuncredef()){
            cout<<"函数"<<p.id<<"重复定义\n";
        }
        else{
            cout<<"函数"<<p.id<<"已定义\n";
        }
        return;
    }
    else {
        cout<<"函数 "<<p.id<<" 未定义\n";
    }
}
bool inbigbracket(){
    if(curw.id=="{"){
        sign.c=curw.id;sign.column=curw.col;sign.row=curw.row;
        twin.push(sign);
        //入新set
        if(dyhanshuti==0){
            parastack.push(paraset);
            for(auto &p: paraset){
                last.insert(last.begin(),p);
            }
            paraset=newparaset;
            presize=twin.size();
        }
        
        return true;
    }
    return false;
}
//仅当完全符合{配对则返回true
bool outbigbracket(){
    // ifstack=emptystack;
    if(curw.id=="}"){
        if(breakpos)backbreak();
        if(returnpos && twin.size()==1 && twin.top().c=="}")backreturn();
        if(fc)back("f",0);
        isbreakexists=false;
        isreturnexists=false;
        if(twin.empty()){
            cout<<"}处理出错，请检查！";
        }
        else if(twin.top().c=="{"){
            twin.pop();
            //取旧set
            if(dyhanshuti && twin.size()==presize){
                for(auto &p:paraset){
                    last.insert(last.begin(),p);
                }
                if(parastack.empty()){
                    cout<<"parastack出现问题\n";
                }
                else{
                    paraset=parastack.top();
                    parastack.pop();
                }
                
                dyhanshuti=0;
            }
            return true;
        }
        else error(twin.top().row,twin.top().column,"缺少}");
    }
    return false;
}
bool inbracket(){
    if(curw.id=="("){
        sign.c=curw.id;sign.column=curw.col;sign.row=curw.row;
        twin.push(sign);
        return true;
    }
    return false;
}
//仅当完全符合（配对则返回true
bool outbracket(){
    if(curw.id==")"){
        if(twin.empty()){
            cout<<")处理出错，请检查！";
            
        }
        else if(twin.top().c=="("){
            twin.pop();
            return true;
        }
        else error(twin.top().row,twin.top().column,"缺少)");
        
    }
    return false;
}
//各类判断函数

//判断是否为while
bool iswhile(string a){
    if(a.length()==5 && a==xunhuan[0])return true;
    else return false;
}

//判断是否为break
bool isbreak(string a){
    if(a.length()==5 && a== xunhuan[1])return true;
    else return false;
}
//判断是否为逻辑关键词 or，and
bool islj(string a){
    if(a.length()==2 && a==luoji[0])return true;
    else if(a.length()==3 && a==luoji[1])return true;
    else return false;
}
//判断是否为条件关键词 if，else
bool istj(string a){
    if(a.length()==2 && a==tiaojian[0])return true;
    else if(a.length()==4 && a==tiaojian[1])return true;
    else return false;
}
//判断是否为定义关键词 int，float，string
bool isdy(string a){
    if(a.length()==3 && a==dingyigjz[0])return true;
    else if(a.length()==5 && a==dingyigjz[1])return true;
    else if(a.length()==6 && a==dingyigjz[2])return true;
    else return false;
}

//判断是否为id 标识符|整数|小数
bool isid(){
    if(curw.type==1 || curw.type==4 || curw.type==5)return true;
    else return false;
}

//----------------终结符---------------

//标识符
bool bsf(){
    int type=curw.type;
    
    if(type==1) return true;
    else return false;
}

//定义关键字
bool dy(){
    int type=curw.type;
    string y=curw.id;
    
    if(isdy(y)){
        if(type==2)return true;
        else cout<<"--定义关键词类型错误\n";
    }
    return false;
}
//条件关键字
bool tj(){
    int type=curw.type;
    string y=curw.id;
    
    if(istj(y)){
        if(type==2)return true;
        else cout<<"--条件关键词类型错误\n";
    }
    return false;
}
//逻辑关键字
bool lj(){
    int type=curw.type;
    string y=curw.id;
    
    if(islj(y)){
        if(type==2)return true;
        else cout<<"--逻辑关键字类型错误\n";
    }
    return false;
}
//循环关键字
bool xh(){
    int type=curw.type;
    string y=curw.id;
    
    if(iswhile(y)|| isbreak(y)){
        if(type==2)return true;
        else cout<<"--循环类型错误\n";
    }
    return false;
}
//返回关键字
bool rt(){
    int type=curw.type;
    string y=curw.id;
    
    if(y==ret){
        if(type==2)return true;
        else cout<<"--return类型错误\n";
    }
    return false;
}

//字符串
bool zfc(){
    int type=curw.type;
    
    if(type==3) return true;
    return false;
}

//整数
bool zs(){
    int type=curw.type;
    
    if(type==4) return true;
    return false;
}

//小数
bool xs(){
    int type=curw.type;
    
    if(type==5) return true;
    return false;
}

//大括号
bool dkh(){
    int type=curw.type;
    string y=curw.id;
    
    if(y=="{" || y=="}"){
        if(type==6)return true;
        else cout<<"--{}类型错误\n";
    }
    return false;
}
//小括号
bool xkh(){
    int type=curw.type;
    string y=curw.id;
    
    if(y=="(" || y==")"){
        if(type==6)return true;
        else cout<<"--()类型错误\n";
    }
    return false;
}
//分号
bool fh(){
    int type=curw.type;
    string y=curw.id;
    
    if(y==";"){
        if(type==6)return true;
        else cout<<"--;类型错误\n";
    }
    return false;
}
//逗号
bool dh(){
    int type=curw.type;
    string y=curw.id;
    
    if(y==","){
        if(type==6)return true;
        else cout<<"--,类型错误\n";
    }
    return false;
}

//四则运算符
bool szysf(){
    int type=curw.type;
    
    if(type==7) return true;
    return false;
}

//比较运算符
bool bjysf(){
    int type=curw.type;
    if(type==8) return true;
    return false;
}

//赋值运算符
bool fzysf(){
    int type=curw.type;
    if(type==9) return true;
    return false;
}

//----------------非终结符-------------
int dyfuncnameflag=0;
//定义语句
void dingyi(){
    dyfg=1;
    string s=curw.id;
    if(bsf()){
        token p=curw;
        fzid=curw.id;
        if(dyfuncnameflag==0 && funccall==0){funcname=curw.id;dyfuncnameflag=1;}
        read();
        if(dyhanshu)++hanshuparanum;
        if(!xkh() ){
            if(ispararedef(p)){
                cout<<"变量"<<p.id<<"重复定义\n";
            }
            else{
                paraset.insert(s);
                paramap.insert({s,0});    
            }
            
        }
        dingA();
        //函数定义内容
        if(inbigbracket()){
            dyhanshuti=1;
        }
    }
    else{error(curw.row,curw.col,"定义对象不是标识符");}
    
}

//区分函数、赋值、多定义
void dingA(){
    
    int fg=0;
    //函数定义
    if(curw.id=="("){
        if(dyhanshu==0){paraset=newparaset;}
        dyfg=1;
        dyhanshu=1;
        hanshuparanum=0;
        if(inbracket())fg=1;
        read();
        if(dy()){
            read();
            ++hanshuparanum;
            dingyi();
        }
        else if(curw.id!=")"){error(curw.row,curw.col,"多定义缺少定义关键词");}
        if(fg){
            outbracket();read();
            if(isfuncredef()){
                cout<<"函数"<<funcname<<"重复定义\n";
            }
            else{
                funcset.insert(funcname);
                funcmap[funcname]=hanshuparanum;
            }
            dyfuncnameflag=0;
            cout<<"\n*********以下为函数"<<funcname<<"内的情况*********\n";
        }
    }
    //赋值
    else if(curw.id=="="){
        if(judgepdfuzhi){error(curw.row,curw.col,"赋值=符号滥用");return;}
        judgepdfuzhi=1;
        read();
        fuzhi();
    }
    //多定义 ：dingB需要,判断
    else if(curw.id==","){
        dingB();
    }
    else {dyfg=1;return;}
}

//多个定义
void dingB(){
    
    if(dh()){
        read();
        //定义
        if(funccall==0){
            
            if(dy())read();
            // read();
            dyfg=1;
            if(bsf()){
                if(!ispararedef()){
                    paraset.insert(curw.id);
                    paramap.insert({curw.id,0});
                }
                else{
                    cout<<"变量"<<curw.id<<"重复定义\n";
                }
                fzid=curw.id;
                read();dingC();
            }
            else{
                error(curw.row,curw.col,"定义的，后面不是标识符");
            }
        }
        //函数调用
        else if(isid() && funccall){
            isparadef();
            ++usehanshuparanum;
            putquat("par","_","_",curw.id);
            paramap[curw.id]=1;
            read();
            if(curw.id==",")
                dingB();
        }
        else if(funccall){error(curw.row,curw.col,"多参数缺少定义关键词");}
        else{error(curw.row,curw.col,"多定义缺少定义关键词");}
        
    }
    // else if(funccall){error(curw.row,curw.col,"逗号后无整数、小数、标识符");}
    return;
}
//多个赋值定义
void dingC(){
    int fg=0;
    if(fzysf()){
        if(judgepdfuzhi){error(curw.row,curw.col,"赋值=符号滥用");return;}
        judgepdfuzhi=1;
        read();
        fuzhi();
        dingB();
    }
    else dingB();
}

//全语句
void fullsen(){
    ifhwhilefg=false;
    partialsen();
    if(ifhwhilefg)return;
    if(dyhanshuti&&curw.id=="{"){inbigbracket();read();return;}
    else if(curw.id=="}"){outbigbracket();read();return;}
    else if(curw.id==")"){outbracket();read();error(curw.row,curw.col,"fullsen处)出问题");}
    else if(!fh()){error(curw.row,curw.col,"句子不以;结尾");}
    if(curw.id==";"){
        dyfg=0;
        read();
    }
    judgepdfuzhi=0;
}

//while语句
void xunhuansen(){
    iforwhile=1;
    prejunc="";
    read();
    if(inbracket()){
        read();
        ifwhile=1;
        while(curw.id!=")"){
            partialsen();
        }
        if(ifwhile)back("t");
        if(!outbracket()){error(curw.row,curw.col,"while的括号少 )");}
        else read();
        ifwhile=0;
        if(inbigbracket())read();
    }
    else{
        error(curw.row,curw.col,"while未跟(");
    }
    if(!inbigbracket())partialsen();
    if(curw.id==";")read();
    judgepdfuzhi=0;

}
//if语句
void ifsen(){
    prejunc="";
    iforwhile=0;
    //ifhwhilefg=true;
    if(curw.id=="if"){
        ifstack.push("if");
        read();
        if(inbracket()){
            read();
            ifwhile=1;
            while(curw.id!=")"){
                partialsen();
            }
            if(ifwhile)back("t",0);
            if(!outbracket()){error(curw.row,curw.col,"if没有后)");}
            else read();
            ifwhile=0;
        }
        else{error(curw.row,curw.col,"if没有前(");}
    }
    else if(curw.id=="else"){
        read();
        if(curw.id=="if"){//else if
            if(!ifstack.empty()){
                if(ifstack.top()!="if"){error(curw.row,curw.col,"else前没有if");}
                else{
                    read();
                    ifwhile=1;
                    back("f",0);
                    if(inbracket()){
                        read();
                        while(curw.id!=")"){
                            partialsen();
                        }
                        
                        if(ifwhile)back("t",0);
                        if(!outbracket()){error(curw.row,curw.col,"if没有后)");}
                        else read();
                    }
                    else{error(curw.row,curw.col,"else if没有前(");}
                    if(!inbigbracket())ifhwhilefg=true;
                    else read();
                    ifwhile=0;
                }
            }
            else{error(curw.row,curw.col,"else前没有if");}
        }
        else{//单个else
            if(!ifstack.empty()){
                if(ifstack.top()!="if"){error(curw.row,curw.col,"else前没有if");}
                else{
                    back("f",0);//保险起见
                    if(!inbigbracket())ifhwhilefg=true;
                    else read();
                    ifstack.pop();
                }
            }
            else{error(curw.row,curw.col,"else前没有if");}

        }

    }
    else{
        error(curw.row,curw.col,"没有先出现if");return;
    }
    if(curw.id==";")read();
    judgepdfuzhi=0;

}

stack<string>pp,op,emptyp;
//处理括号内容
void operatexkh(){
    int fg=0;
    if(inbracket()){fg=1;read();}
    while(curw.id!=")"){
        if(curw.id=="("){
            op.push("(");
            operatexkh();
        }
        else if(isid()){
            isparadef();
            pp.push(curw.id);
        }
        else if(szysf()){
            //做乘除
            if(curw.id=="*" ||curw.id=="/"){
                op.push(curw.id);
                read();
                if(curw.id=="("){
                    operatexkh();
                }
                b=pp.top();pp.pop();a=pp.top();pp.pop();
                pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
            }
            //+ -
            else {
                if(!op.empty()&&(op.top()=="-" ||op.top()=="+") &&(curw.id=="-" ||curw.id=="+")){
                    b=pp.top();pp.pop();a=pp.top();pp.pop();
                    pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
                }
                op.push(curw.id);
            }
        }
        //空符号，则）不来自你
        if(ifwhile && op.empty() && curw.id==")")break;
        else read();
        if(curw.id==")"){
            if(!op.empty()&&(op.top()=="-" ||op.top()=="+")){
                b=pp.top();pp.pop();a=pp.top();pp.pop();
                pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
            }

        }
    }
    if(fg && !outbracket()) cout<<"\n计算小括号符号出问题\n";
}

//四则
void sizeyunsuan(){
    szbjfg=1;
    if(!szysf())return;
    pp.push(szid);
    while(szysf()|| xkh() ||isid()){
        if(curw.id=="("){
            op.push("(");
            operatexkh();
            if(!outbracket()) cout<<"\n计算小括号符号出问题\n";
        }
        else if(isid()){
            isparadef();
            pp.push(curw.id);
            paramap[curw.id]=1;
        }
        else if(szysf()){
            //做乘除
            if(curw.id=="*" ||curw.id=="/"){
                op.push(curw.id);
                read();
                if(xkh()){
                    operatexkh();
                }
                else{
                    pp.push(curw.id);
                }
                b=pp.top();pp.pop();a=pp.top();pp.pop();
                pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();

            }
            else {
                if(!op.empty()&&(op.top()=="-" ||op.top()=="+")){
                    b=pp.top();pp.pop();a=pp.top();pp.pop();
                    pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();

                }
                op.push(curw.id);
            }
        }
        
        if(!op.empty() && op.top()=="(" && curw.id==")"){
            if(!op.empty()){
                if(op.top()=="-" ||op.top()=="+"){
                    b=pp.top();pp.pop();a=pp.top();pp.pop();
                    pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
                }
                else if(op.top()=="("){
                    op.pop();
                }
            }
        }
        //比较混乱
        else if((curw.id==")" || curw.id=="+"|| curw.id=="-") && op.size() &&pp.size()>=2){
            b=pp.top();pp.pop();a=pp.top();pp.pop();
            pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
        }
        //空符号，则）不来自你
        if(ifwhile && op.empty() && curw.id==")")break;
        else read();
    }
    if(!op.empty()){
        if(op.top()=="-" ||op.top()=="+"){
            b=pp.top();pp.pop();a=pp.top();pp.pop();
            pp.push("T"+to_string(++Tnum));putquat(op.top(),a,b,"T"+to_string(Tnum));op.pop();
        }
        else error(curw.row,curw.col,"四则运算顺序有误,op剩余必须为+-");
    }
    op=pp=emptyp;
    return;
}
//现在逻辑连接符号与之前是否相同,同为0，异为1
int isdiffprejunc(){
    if(prejunc.length()<2)return 0;
    else if(prejunc[0]==prejunc[1])return 0;
    return 1;
}
//比较
void bijiao(){
    int preN=Tnum;
    string sign=curw.id;
    if(bjysf()){
        read();
        if(isid()){
            isparadef();
            string lastid=curw.id;
            //比较的后操作数为四则运算
            szbjfg=0;
            read();
            if(szysf())manip();
            // if(lj()){
                if(andfg && orfg==0){
                    //四则运算后
                    if(szbjfg)
                        putquat("j"+sign,bjid,"T"+to_string(Tnum));
                    //仅标识符
                    else
                        putquat("j"+sign,bjid,lastid);

                    if(isdiffprejunc())back("t");
                    else quatlist[tc].pos=to_string(sennum-1);
                    tc=sennum-1;
                    putquat("j","_","_",to_string(fc));
                    fc=sennum-1;
                }
                else if(orfg && andfg==0){
                    //四则运算后
                    if(szbjfg)
                        putquat("j"+sign,bjid,"T"+to_string(Tnum),to_string(tc));
                    //仅标识符
                    else
                        putquat("j"+sign,bjid,lastid,to_string(tc));

                    tc=sennum-1;
                    putquat("j","_","_");
                    if(isdiffprejunc())back("f");
                    else quatlist[fc].pos=to_string(sennum-1);
                    fc=sennum-1;
                }
                else if(andfg==0 && orfg==0){
                    //四则运算后
                    if(szbjfg)
                        putquat("j"+sign,bjid,"T"+to_string(Tnum),to_string(tc));
                    //仅标识符
                    else
                        putquat("j"+sign,bjid,lastid,to_string(tc));
                    tc=sennum-1;
                    putquat("j","_","_");
                    fc=sennum-1;
                }
                else {
                    cout<<"\nand和or的处理有误\n";
                }
            // }
            bijiaoA();
        }
    }
    else if(fzysf()){
        int fg=0;
        if(judgepdfuzhi){error(curw.row,curw.col,"赋值=符号滥用");return;}
        read();
        if(isid()){//fuzhi需要id打头
            isparadef();
            judgepdfuzhi=1;
            fuzhi();

        }
    }
    // 一般不会出现
    else{error(curw.row,curw.col,"有比较运算符却被判非法");}
    szbjfg=0;
}
//多个比较逻辑
void bijiaoA(){
    andfg=orfg=0;
    if(curw.id=="and"){
        andfg=1;orfg=0;
        prejunc.insert(prejunc.begin(),'1');
        read();
        if(isid()){
            isparadef();
            if(bsf())szid=bjid=fzid=curw.id;

            read();
            manip();
        }
    }
    else if(curw.id=="or"){
        andfg=0;orfg=1;
        prejunc.insert(prejunc.begin(),'0');
        read();
        if(isid()){
            isparadef();
            if(bsf())szid=bjid=fzid=curw.id;
            read();
            manip();
        }
    }
    
    else return;
}

//赋值
void fuzhi(){
    if(isid()|| curw.id=="("){
        int leftkhfg=0,fg=0;
        isparadef();
        if(curw.id=="("){leftkhfg=1;read();fg=1;}
        if(isid()){
            if(bsf())paramap[curw.id]=1;
            boolid=bjid=szid=curw.id;
            read();
            if(fh())
                putquat("=",bjid,"_",fzid);
        }
        // if(leftkhfg==2)cout<<"error 赋值处的szid有误\n";
        if(szysf()){
            if(leftkhfg)op.push("(");
            sizeyunsuan();
            putquat("=","T"+to_string(Tnum),"_",fzid);
        }
        else if(bjysf()){
            bijiao();
        }
        else if(lj()){
            bijiaoA();
        }
        fzid=boolid=bjid=szid="";
    }
    else if(dh())return;
    else {error(curw.row,curw.col,"赋值=,标识符或整数或小数后非四则or比较");}
    
}
// 跳转return操作
inline void backreturn(){
    if(isreturnexists ){
        if(dyhanshuti)
            quatlist[returnpos].pos=to_string(sennum);
        else{
            error(curw.row,curw.col,"return不在函数体里面");
        }
    }
}
// 跳转break操作
inline void backbreak(){
    if(isbreakexists ){
        if(iforwhile)
            quatlist[breakpos].pos=to_string(sennum);
        else{
            error(curw.row,curw.col,"break不在while里面");
        }
    }
    
}
//部分语句 return break 定义 kong manip fuzhi
void partialsen(){
    int flag=0;
    if(inbracket()){read();flag=1;}
    if(rt()) {
        read();partialsen();
        returnpos=sennum;isreturnexists=true;
        putquat("j","_","_");return;
    }
    else if(isbreak(curw.id)){
        read();
        isbreakexists=true;breakpos=sennum;
        putquat("j","_","_");return;
    }
    else if(dy()) {read();dingyi();dyfuncnameflag=0;}
    else if(isid()) {

        if(zs()||xs()){
            fzid=boolid=bjid=szid=curw.id;
            read();manip();
        }
        else if(bsf()){
            token p=curw;
            boolid=bjid=szid=fzid=curw.id;
            read();
            int fg=0;
            //=
            if (curw.type == 9){
                isparadef(p);
                if (judgepdfuzhi)error(curw.row, curw.col, "赋值=符号滥用");
                judgepdfuzhi=1;
                paramap[fzid]=1;
                read();
                fuzhi();

            }
            else {//hanshu / szys / bj
                if(curw.id=="(")isfuncdef(p);
                else if(szysf())isparadef();
                else cout<<"\n待定——partialsen\n";
                funcname=p.id;
                manip();
                
            }
        }
        else{error(curw.row,curw.col,"isid函数有误");}
        
    }
    else if(lj()){
        bijiaoA();
    }
    while(dh()){
        read();
        if(isid()){
            isparadef();
            fzid=boolid=bjid=szid=curw.id;
            read();manip();
        }
    }
    if(curw.id==")" && flag){outbracket();read();}
    judgepdfuzhi=0;

}
//四则+比较+函数调用
void manip(){
    if(curw.id==";"|| dh()){return;}
    else if(curw.id=="("){
        read();
        //函数调用开始
        funccall=1;
        if(isid()){
            isparadef();
            putquat("par","_","_",curw.id);
            paramap[curw.id]=1;
            usehanshuparanum=1;
            read();dingB();
            if(curw.id==")"){
                putquat("call","_","_",funcname);
                if(funcmap[funcname]==usehanshuparanum)cout<<"\n函数调用"<<funcname<<"形参和实参个数相等\n";
                else {
                    cout<<"\nWarning : 函数调用"<<funcname<<"形参和实参个数不相等\n";
                    //未定义函数以示区分
                    funcmap[funcname]=-1;
                }
                read();
            }
            else{error(curw.row,curw.col,"函数调用后少)");}

        }
        else if(curw.id==")"){
            putquat("call","_","_",funcname);
            if(funcmap[funcname]==usehanshuparanum)cout<<"\n函数调用"<<funcname<<"形参和实参个数相等\n";
            else {
                cout<<"\nWarning : 函数调用"<<funcname<<"形参和实参个数不相等\n";
                //未定义函数以示区分
                funcmap[funcname]=-1;
            }
            read();
        }
        // else{error(curw.row,curw.col,"函数调用的参数不合法");}
    }
    else if(szysf()){
        sizeyunsuan();
        bjid="T"+to_string(Tnum);
        if(ifwhile){
            if(bjysf()){
                manip();
            }
        }
    }
    else if(bjysf())bijiao();
    //单标识符逻辑
    else if(ifwhile && lj()){
        putquat("jnz",bjid,"_");tc=sennum-1;
        putquat("j","_","_");fc=sennum-1;
        bijiaoA();
    }
    
    fzid=boolid=bjid=szid="";
    // else{error(curw.row,curw.col,"manip调用错误");}
}

int main(){
    char c;
    bool first=true;
    if(!file){
        cout<<"Fail to open. please try again!";
        exit(-1);
    }
    else{
        cout<<"\nsuccess!\n\n";
        while(file.peek()!=EOF){
            if(first){read();first=false;}//首次读取
            dyfg=iforwhile=szbjfg=dyhanshu=dyfuncnameflag=hanshuparanum=ifwhile=funccall=judgepdfuzhi=0;
            boolid=fzid=bjid=szid=funcname="";
            if(iswhile(curw.id))
                xunhuansen();
            else if(tj())
                ifsen();
            else if(curw.id=="}"){
                if(!outbigbracket()){
                    flag=false;cout<<curw.row<<' '<<curw.col<<"\t词法分析器没分析出不成对{}\n";read();
                }
                else read();
            }
            else fullsen();
        }
        cout<<"\n\n*********扫描分析完毕*********\n";
        printall();
        cout<<"\n已定义变量如下:\n";
        for(auto i=last.begin();i!=last.end();i++){
            cout<<(*i)<<'\t';
        }
        cout<<"\n\n未使用变量如下:\n";
        for(auto i=paramap.begin();i!=paramap.end();i++){
            if(i->second==0)cout<<i->first<<'\t';
        }
        cout<<"\n\n已定义函数及参数个数如下:";
        for(auto i=funcset.begin();i!=funcset.end();i++){
            cout<<'\n'<<(*i)<<"有"<<funcmap[*i]<<"个形参";
            if(funcmap[*i]==-1)
                cout<<" ———— 未定义函数";
        }
        file.close();
    }
    return 0;
}