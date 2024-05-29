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
typedef struct word
{
    int   type;
    string  id;
    int   row;
    int   col;
}token;
typedef struct{
    int row,column;
    string c;
}unit;

ifstream file("output_words.txt",ios::in);
//if的顺序判断
stack<string>ifstack,emptystack;
//预读队列
queue<token>prelist;
//{} ()判断
stack<unit> twin;

int judgepd=0;//1为已有赋值，0为未有
int hanshu=0;//0为定义，1为函数调用
bool flag=true,ifhwhilefg;
unit sign=*(new unit);
token curw=*(new token{0," ",0,0});

bool readbraflag;
string str;
// stringstream ss;
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
//报错(row,col,文字报错类型) 此刻curw为;下一位
void error(int i,int j,string errortype="未知"){
    flag=false;
    cout<<endl<<"Fault: in row "<<i<<" column "<<j<<" 报错原因: "<<errortype<<"\t------ 忽略后续至下一个语句 <从 ";
    while(curw.id!=";" && curw.id!="{" &&curw.id!="(" &&curw.id!=")"&&curw.id!="}"){
        read();
    }
    cout<<curw.id<<" 开始>\n";
}
bool inbigbracket(){
    if(curw.id=="{"){
        sign.c=curw.id;sign.column=curw.col;sign.row=curw.row;
        twin.push(sign);
        return true;
    }
    return false;
}
//仅当完全符合{配对则返回true
bool outbigbracket(){
    ifstack=emptystack;
    if(curw.id=="}"){
        if(twin.empty()){
            cout<<"}处理出错，请检查！";
        }
        else if(twin.top().c=="{"){
            twin.pop();
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

//定义语句
void dingyi(){
    if(bsf()){
        read();
        dingA();
    }
    else{error(curw.row,curw.col,"定义对象不是标识符");}
}

//区分函数、赋值、多定义
void dingA(){
    int fg=0;
    if(xkh()){
        if(inbracket())fg=1;
        read();
        if(dy()){
            read();
            dingyi();
        }
        else if(curw.id!=")"){error(curw.row,curw.col,"多定义缺少定义关键词");}
        if(fg){outbracket();read();}
    }
    else if(curw.id=="="){
        if(judgepd){error(curw.row,curw.col,"赋值=符号滥用");return;}
    judgepd=1;
        read();fuzhi();
    }
    else if(curw.id==","){//dingB需要,判断
        dingB();
    }
    else return;
}

//多个定义
void dingB(){
    if(dh()){
        read();
        //定义
        if(dy() && hanshu==0){
            read();
            if(bsf()){
                read();dingC();
            }
            else{
                error(curw.row,curw.col,"定义的，后面不是标识符");
            }
        }
        //函数调用
        else if(isid() && hanshu){
            read();
            if(curw.id==",")
                dingB();
        }
        else if(hanshu){error(curw.row,curw.col,"多参数缺少定义关键词");}
        else{error(curw.row,curw.col,"多定义缺少定义关键词");}
        
    }
    else if(hanshu){error(curw.row,curw.col,"逗号后无整数、小数、标识符");}
    return;
}
//多个赋值定义
void dingC(){
    if(fzysf()){
        if(judgepd){error(curw.row,curw.col,"赋值=符号滥用");return;}
        judgepd=1;
        read();
        fuzhi();
        dingB();
    }
    else dingB();
}

//四则
void sizeyunsuan(){
    if(szysf()){
        read();
        if(isid()){
            read();sizeyunsuan();
        }
        else{error(curw.row,curw.col,"四则运算符后少运算数");}
    }
    else return;
}
//比较
void bijiao(){
    if(bjysf()){
        read();
        if(isid()){
            read();
            bijiaoA();
        }
    }
    else if(fzysf()){
        if(judgepd){error(curw.row,curw.col,"赋值=符号滥用");return;}
        read();
        if(isid()){//fuzhi需要id打头
            judgepd=1;
            fuzhi();
        }
    }
    // 一般不会出现
    else{error(curw.row,curw.col,"有比较运算符却被判非法");}
}
//多个比较逻辑
void bijiaoA(){
    if(curw.id=="and"){
        read();
        if(curw.id=="and"){
            read();
            if(isid()){
                read();
                bijiao();
            }
        }
        else{error(curw.row,curw.col,"词法分析未筛除单独&");}

    }
    else if(curw.id=="or"){
        read();
        if(curw.id=="or"){
            read();
            if(isid()){
                read();
                bijiao();
            }
        }
        else{error(curw.row,curw.col,"词法分析未筛除单独|");}
    }
    else return;
}

//赋值
void fuzhi(){
    if(isid()){
        read();
        if(szysf()){
            sizeyunsuan();
        }
        else if(bjysf()){
            bijiao();
        }
        else if(lj()){
            bijiaoA();
        }
    }
    else if(dh())return;
    else {error(curw.row,curw.col,"赋值=,标识符或整数或小数后非四则or比较");}
    
}


//全语句
void fullsen(){
    ifhwhilefg=false;
    partialsen();
    if(ifhwhilefg)return;
    if(curw.id=="{"){inbigbracket();read();return;}
    else if(curw.id=="}"){outbigbracket();read();return;}
    else if(!fh()){error(curw.row,curw.col,"句子不以;结尾");}
    if(curw.id==";")read();
    judgepd=0;
}
//while语句
void xunhuansen(){
    read();
    if(inbracket()){
        read();
        partialsen();
        if(!outbracket()){error(curw.row,curw.col,"while的括号少 )");}
        else read();
        if(inbigbracket())read();
        
    }
    else{
        error(curw.row,curw.col,"while未跟(");
    }
    if(!inbigbracket())partialsen();
    if(curw.id==";")read();
    judgepd=0;

}
//if语句
void ifsen(){
    //ifhwhilefg=true;
    if(curw.id=="if"){
        ifstack.push("if");
        read();
        if(inbracket()){
            read();
            partialsen();
            if(!outbracket()){error(curw.row,curw.col,"if没有后)");}
            else read();
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
                    if(inbracket()){
                        read();
                        partialsen();
                        if(!outbracket()){error(curw.row,curw.col,"if没有后)");}
                        else read();

                    }
                    else{error(curw.row,curw.col,"else if没有前(");}
                    if(!inbigbracket())ifhwhilefg=true;
                    else read();
                }
            }
            else{error(curw.row,curw.col,"else前没有if");}
        }
        else{//单个else
            if(!ifstack.empty()){
                if(ifstack.top()!="if"){error(curw.row,curw.col,"else前没有if");}
                else{
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
    judgepd=0;

}
//部分语句 return break 定义 kong manip fuzhi
void partialsen(){
    int flag=0;
    if(inbracket()){read();flag=1;}
    if(rt()) {read();partialsen();}
    else if(isbreak(curw.id)){read();return;}
    else if(dy()) {read();dingyi();}
    else if(isid()) {

        if(zs()||xs()){
            read();manip();
        }
        else if(bsf()){
            read();
            if (curw.type == 9){
                if (judgepd)error(curw.row, curw.col, "赋值=符号滥用");
                judgepd=1;
                read();
                fuzhi();
            }
            else{
                manip();
            }
        }
        else{error(curw.row,curw.col,"isid函数有误");}
    }
    while(dh()){
        read();
        if(isid()){
            read();manip();
        }
    }
    if(curw.id==")" && flag){outbracket();read();}
    judgepd=0;

}
//四则+比较+函数调用
void manip(){
    if(curw.id==";"|| dh()){return;}
    else if(curw.id=="("){
        read();
        hanshu=1;
        if(isid()){
            read();dingB();
            if(curw.id==")"){
                read();
            }
            else{error(curw.row,curw.col,"函数调用后少)");}

        }
        else{error(curw.row,curw.col,"函数调用的参数不合法");}
    }
    else if(szysf()){
        sizeyunsuan();
    }
    else if(bjysf())bijiao();
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
            hanshu=0;
            judgepd=0;
            if(iswhile(curw.id))xunhuansen();
            else if(tj())ifsen();
            else if(curw.id=="}"){
                if(!outbigbracket()){
                    flag=false;cout<<curw.row<<' '<<curw.col<<"\t词法分析器没分析出不成对{}\n";read();
                }
                else read();
            }
            else fullsen();
        }
        cout<<boolalpha<<"\n\n\n语法分析结果: "<<flag<<endl;
        cout<<"\n*********语法分析完毕*********";
        file.close();
    }
    return 0;
}
