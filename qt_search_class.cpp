#include "qt_search_class.h"
#include <memory>
#include "result.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QTextEdit>


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

//static global vars->
//
//int *result_array()=NULL;
	//static int res_size=0;
    //static int *result_arr_size()=&res_size;
//static std::unique_ptr<int> result_arr_size()=std::make_unique<int>(res_size);
//result_arr_size()=&res_size;
    //static int* result_array()=new int[*result_arr_size()]; // array of result
	//static char *pat = new char[254]; // pattern for search
//int q=0;
//<-static global vars

//File is empty func
bool Search::is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}
//

int Search::clear_memory()
{
 delete[] result_array();
 result_array()=NULL;
 //delete[] pat;
 //pat=NULL;
 return 0;
}



size_t Search::strlenpp(const std::string *str)
{
	const char * str_temp=str->c_str();
    size_t t = 0;
    for (; str_temp[t]!='\0';++t);
    return t;

}


void Search::show_mas(size_t number_of_symbols){ // result pattern indexes in text

//std::string temp="";
//for(size_t i=0;i<*result_arr_size();i++){
//    //std::cout << result_array()[i];
//    temp+=std::to_string(result_array()[i]);
//    if(i!=*result_arr_size()-1) temp+=", ";//std::cout << ", ";
//    else temp+=".";//std::cout << ".";
//}
//if(*result_arr_size()!=0)temp+="\n";//std::cout<<std::endl;
////else std::cout<<RED<<"Entries not found.:("<<RESET<<std::endl;
//else {
//    //temp+=RED;

//    //temp+="<div><font color=\"red\">Entries not found.:(</font></div>";
//    temp+="Entries not found.:(";

//    //temp+=RESET;

//    temp+="\n";
//}
//QString result = QString::fromStdString(temp);

QString result="";
for(size_t i=0;i<*result_arr_size();i++){
    //std::cout << result_array()[i];
    result+=QString::number(result_array()[i]);
    if(i!=*result_arr_size()-1) result+="<font color=\"black\">, </font>"; //std::cout << ", ";
    else result+="<font color=\"black\">.</font>"; // std::cout << ".";
}
if(*result_arr_size()!=0)result+="\n";//std::cout<<std::endl;
//else std::cout<<RED<<"Entries not found.:("<<RESET<<std::endl;
else {
    //result+=RED;

    //result+="<div><font color=\"red\">Entries not found.:(</font></div>";
    result+="Entries not found.:(";

    //result+=RESET;

    result+="\n";
}

QString head="<br>Pattern exist in next literal positions(from zero):";

QString html_final="";

QString q_number_of_symbols = "Number of bytes (UTF-8 symbols) in file: <font color=\"red\">" + QString::number(number_of_symbols) + "</font>.";

if(*result_arr_size()==0)
html_final = "<div><font color=\"red\">"+QString(result)+"</font></div>";
else
html_final = "<div><font color=\"green\">"+QString(result)+"</font></div>";

  QMessageBox::about(0,"Search results", q_number_of_symbols+head+html_final);

//_Exit (EXIT_SUCCESS);
}







//void Search::show_mas(void){ // result pattern indexes in text
//for(int i=0;i<*result_arr_size();i++){
//	std::cout << result_array()[i];
//	if(i!=*result_arr_size()-1) std::cout << ", ";
//	else std::cout << ".";
//}
//if(*result_arr_size()!=0)std::cout<<std::endl;
//else std::cout<<RED<<"Entries not found.:("<<RESET<<std::endl;
// //_Exit (EXIT_SUCCESS);
//}

void Search::show_res(const char*text,const char*pattern) //result pattern text (with '\n') -- separate fragments
{
if(!(*result_arr_size())){std::cout <<RED<< "no result"<<RESET<<std::endl; return;}
const char*temp_pat=NULL;temp_pat=pattern;
int res_i=0;
    for(size_t i=0;i<strlenpp(text);++i){

           if (i==result_array()[res_i]){
		   while(*temp_pat){
	  std::cout<<GREEN<<text[i]<<RESET;
		   ++i;++temp_pat;
		   }
		   //std::cout<<text[i];
	--i;
temp_pat=pattern;//continue;
std::cout<<std::endl;
++res_i;
	   }

	}

}

//void Search::show_finale(const std::string& text,const char*pattern)
//{
//	int start_pos=0;
//	//int i=0;
//	int text_find=0;
        
////	text_find=text.find(pattern,start_pos);
////	std::cout << "text_find== "<<text_find<<'\n';
////std::cout << BLACK << text.substr(start_pos,text_find-start_pos) <<RESET;
//        //text_find=text.find(pattern,start_pos);
//        for(size_t i=0;i!=(*result_arr_size());++i)
//	{
//		  text_find=text.find(pattern,start_pos);
//		//std::cout << text_find << std::endl;
	
//std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
  

//     std::cout << RED << text.substr(result_array()[i],strlenpp(pattern)) << RESET;
//    // std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
//  start_pos=result_array()[i]+strlenpp(pattern);

//	}
//	std::cout << BLACK << text.substr(start_pos) <<RESET;
//	//std::cout << "\n|text find == "<<text_find ;
//	//std::cout << "|cur start position is == "<<start_pos ;
//	//std::cout << "| length of string == "<<text.length();
//	std::cout<<std::endl;
//}



//QString Search::show_finale(std::string const & text, char const *pattern)
//{
//    QString temp="";

//    size_t start_pos=0;
//    //int i=0;
//    size_t text_find=0;

//        //	text_find=text.find(pattern,start_pos);
//        //	std::cout << "text_find== "<<text_find<<'\n';
//        //std::cout << BLACK << text.substr(start_pos,text_find-start_pos) <<RESET;
//        //text_find=text.find(pattern,start_pos);
//        for(size_t i=0;i!=(*result_arr_size());++i)
//    {
//          text_find=text.find(pattern,start_pos);
//        //std::cout << text_find << std::endl;
////temp+="<pre>";
//        //std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
//temp+="<font color=\"black\">"+QString(QString::fromStdString(text.substr(start_pos,text_find - start_pos)))+"</font>";
//        //temp+=QString(QString::fromStdString(text.substr(start_pos,text_find - start_pos)));


//     //std::cout << RED << text.substr(result_array()[i],strlenpp(pattern)) << RESET;
//temp+="<font color=\"red\">"+QString(QString::fromStdString(text.substr(result_array()[i],strlenpp(pattern))))+"</font>";

//    // std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
//start_pos=result_array()[i]+strlenpp(pattern);

//    }
//    //std::cout << BLACK << text.substr(start_pos) <<RESET;
//temp+="<font color=\"black\">"+QString(QString::fromStdString(text.substr(start_pos)))+"</font>";
//    //temp+=QString(QString::fromStdString(text.substr(start_pos)));

//    //std::cout << "\n|text find == "<<text_find ;
//    //std::cout << "|cur start position is == "<<start_pos ;
//    //std::cout << "| length of string == "<<text.length();
////temp+="</pre>";
//    //std::cout<<std::endl;
//    //temp+="<br>";
//temp.replace(QString("\n"), QString("<br>"));
//    temp.replace(" " , "&#160;");

//std::cout << (int)text[789] << " " << (int)text[790] << " " << (int)text[791] << " " << (int)text[792] << " " << (int)text[793]
//                                            << " "  << (int)text[794] << " " << (int)text[795] << "\n";
//    return temp;
//}



QString Search::show_finale(std::string const & text, char const *pattern)
{
    QString temp="";

    size_t start_pos=0;
    //int i=0;
    size_t text_find=0;

        //	text_find=text.find(pattern,start_pos);
        //	std::cout << "text_find== "<<text_find<<'\n';
        //std::cout << BLACK << text.substr(start_pos,text_find-start_pos) <<RESET;
        //text_find=text.find(pattern,start_pos);
        for(size_t i=0;i!=(*result_arr_size());++i)
    {
          text_find=text.find(pattern,start_pos);
        //std::cout << text_find << std::endl;
//temp+="<pre>";
        //std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
temp+=QString::fromStdString(text.substr(start_pos,text_find - start_pos));
temp.replace(QString("\n"), QString("<br>"));
   temp.replace(QString(" "), QString("&nbsp;"));
   temp = "<font color=\"red\">"+temp+"</font>";
        //temp+=QString(QString::fromStdString(text.substr(start_pos,text_find - start_pos)));


     //std::cout << RED << text.substr(result_array()[i],strlenpp(pattern)) << RESET;
temp+=QString("<pre" +QString::fromStdString(text.substr(result_array()[i],strlenpp(pattern)))+"</pre>");

    // std::cout << BLACK << text.substr(start_pos,text_find - start_pos) <<RESET;
start_pos=result_array()[i]+strlenpp(pattern);

    }
    //std::cout << BLACK << text.substr(start_pos) <<RESET;
temp+=QString("<pre" +QString::fromStdString(text.substr(start_pos))+"</pre>");
    //temp+=QString(QString::fromStdString(text.substr(start_pos)));

    //std::cout << "\n|text find == "<<text_find ;
    //std::cout << "|cur start position is == "<<start_pos ;
    //std::cout << "| length of string == "<<text.length();
//temp+="</pre>";
    //std::cout<<std::endl;
    //temp+="<br>";
//temp.replace(QString("\n"), QString("<br>"));
    //temp.replace(" " , "&#160;");

//std::cout << (int)text[789] << " " << (int)text[790] << " " << (int)text[791] << " " << (int)text[792] << " " << (int)text[793]
//                                            << " "  << (int)text[794] << " " << (int)text[795] << "\n";
    return temp;
}



std::vector<QString> Search::show_finale_qt(std::string const & text, char const *pattern)
{
    std::vector<QString> temp;

    size_t start_pos=0;
    size_t v = 0;
    size_t text_find=0;
    size_t number=0;

//        for(size_t i=0;i!=(*result_arr_size());++i)
do    {
          text_find=text.find(pattern,start_pos);
//std::cout << "text_find in beginning = " << text_find << "\n";
//std::cout << start_pos<<"\n";
            if(text_find==std::string::npos)break;
            ++number;
temp.push_back(QString::fromStdString(text.substr(start_pos,text_find - start_pos)));
    temp[v].replace(QString("\n"), QString("<br>"));
    temp[v].replace(QString(" "), QString("&nbsp;"));
    temp[v] = "<font color=\"black\">"+temp[v]+"</font>";
std::cout << "start_pos from finale: " <<start_pos<<" ";
std::cout << "text_find from finale: " <<text_find<<"\n";
++v;


temp.push_back(QString::fromStdString(text.substr(/*result_array()[i]*/text_find,strlenpp(pattern))));
    temp[v].replace(QString("\n"), QString("<br>"));
    temp[v].replace(QString(" "), QString("&nbsp;"));
    temp[v] = "<font color=\"red\">"+temp[v]+"</font>";
++v;


start_pos=/*result_array()[i]*/text_find+strlenpp(pattern);

    } while(1);
//std::cout << "number == " << number<<"\n";
temp.push_back(QString::fromStdString(text.substr(start_pos)));
    temp[v].replace(QString("\n"), QString("<br>"));
    temp[v].replace(QString(" "), QString("&nbsp;"));
    temp[v] = "<font color=\"black\">"+temp[v]+"</font>";
//std::cout << start_pos << "\n";

    return temp;
}




//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
QString Search::show_finale_qt_test(std::string const & text, char const *pattern)
{
    QString before="";
    QString after="";
    QString main_string="";

    size_t start_pos=0;
 //   size_t v = 0;
    size_t text_find=0;
    size_t number=0;

//        for(size_t i=0;i!=(*result_arr_size());++i)
do    {
          text_find=text.find(pattern,start_pos);
//std::cout << "text_find in beginning = " << text_find << "\n";
//std::cout << start_pos<<"\n";
            if(text_find==std::string::npos)break;
            ++number;
before=QString::fromStdString(text.substr(start_pos,text_find - start_pos));
    before.replace(QString("\n"), QString("<br>"));
    before.replace(QString(" "), QString("&nbsp;"));
    before = "<font color=\"black\">"+before+"</font>";
std::cout << "start_pos from finale: " <<start_pos<<" ";
std::cout << "text_find from finale: " <<text_find<<"\n";
//++v;


after=QString::fromStdString(text.substr(/*result_array()[i]*/text_find,strlenpp(pattern)));
    after.replace(QString("\n"), QString("<br>"));
    after.replace(QString(" "), QString("&nbsp;"));
    after = "<font color=\"red\">"+after+"</font>";
//++v;

    main_string+=before+after;

start_pos=/*result_array()[i]*/text_find+strlenpp(pattern);

    } while(1);
//std::cout << "number == " << number<<"\n";
after=QString::fromStdString(text.substr(start_pos));
    after.replace(QString("\n"), QString("<br>"));
    after.replace(QString(" "), QString("&nbsp;"));
    after = "<font color=\"black\">"+after+"</font>";
//std::cout << start_pos << "\n";
    main_string+=after;
    return main_string;
}










//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Search::show_mas_qt_test(/*size_t number_of_symbols,*/std::string const & text, char const *pattern){ // result pattern indexes in text


QString result="";

  size_t start_pos=0;
  size_t text_find=0;
  size_t number=0;
  size_t strlen = strlenpp(pattern);


do    {
        text_find=text.find(pattern,start_pos);

          if(text_find==std::string::npos){if(number>0)result+="<font color=\"black\">.</font>";break;} // DOT
          else if(number>0) result+="<font color=\"black\">, </font>";                             // COMMA
          ++number;
          result+=QString::number(text_find);

    start_pos=/*result_array()[i]*/text_find+strlen;

        } while(1);

  if(number!=0)result+="<br>";
  else {

      result+="Entries not found.:(";

      result+="<br>";
  }

  QString head="<br>Pattern exist in next literal positions(from zero):";

  QString html_final="";

  //QString q_number_of_symbols = "Number of bytes (UTF-8 symbols) in file: <font color=\"red\">" + QString::number(number_of_symbols) + "</font>.";

  if(number==0)
  html_final = "<div><font color=\"red\">"+QString(result)+"</font></div>";
  else
  html_final = "<font color=\"green\">"+QString(result)+"</font>";

    //QMessageBox::about(0,"Search results", /*q_number_of_symbols+*/ head+html_final);

  Result * r = new Result;

  r->search_results(html_final);

  r->exec();

}













size_t* Search::increase(size_t* data, size_t* m_size, int pointer)
{   
    size_t *more=NULL;
    //++q;
    //if(q==1||q==3)more = (int*)realloc(data, (++*m_size) * sizeof(int));
    more = new size_t[++*m_size];
    //else if(q==0||q==2)more=NULL;
    if(more==NULL){
	   
	    delete[] more;
            more=NULL;
    }else{
        for(size_t i=0;i!=*m_size-1;++i)
		    more[i]=data[i];
    more[*m_size-1]=pointer;
    std::cout << "pointer from increase: " <<pointer<<"\n";
    delete[] data;
    data=NULL;
    data=more;
    more=NULL;
    //*m_size+=1;
    //*data[*m_size-2]=pointer;

    return data;
    }
    std::cout << "realloc error" << std::endl;
    QMessageBox::warning(0, "Realloc error", "Reallocation of memory for result array <font color=\"red\">failed</font>.");
    return data;
};


size_t Search::strlenpp(const char *str)
{
    size_t t = 0;
    for (; str[t]!='\0';++t);
    return t;
}


size_t Search::strlenpp(char *str)
 {
     size_t t = 0;
     for (; str[t]!='\0';++t);
     return t;
}


int Search::strstr_multi(const char *text, const char *pattern)
{
    result_array()[0]=-1;
	if(strlenpp(text)==0){
            //std::cout << "Input c-string is empty" << std::endl;
        QMessageBox::about(0,"Search results", "File is empty. Nothing to search.");
		return 1;}
//    if(strlenpp(pattern)==0){
//        std::cout << "Input pattern is empty" << std::endl;
//        return 1;}
    int p=0;
    int pointer=0;
    size_t number_of_symbols = 0;

    // SEARCH :
  for(size_t t=0;t<=strlenpp(text);++t){
      if(/*text[t]!='\n'&&*/text[t]!='\0')++number_of_symbols; // counter of bytes
      if(pattern[p]==text[t]){                                       //IF 1ST LETTER OF PATTERN IS FIND IN TEXT
          if(pointer==0&&p==0){pointer=t;}
	  if(pattern[p]=='\0'&&text[t]=='\0'){
         result_array()=increase(result_array(),result_arr_size(),pointer); //PATTERN IS FIND AT THE END OF TEXT
                  //show_mas();
		  //return 0; longer then 255 symbols per 1 search entrie.i'm
	            break;     } //{1} +1 to mas,return mass

          ++p;
          continue;
      } else if(pattern[p]=='\0'){
	      //
          result_array()=increase(result_array(),result_arr_size(),pointer); //PATTERN FIND, AND TEXT IS CONTINUE
              
                 p=0;
          pointer=0;
                     //
        		 if(pattern[p]==text[t]){
				 pointer=t;
				 ++p;
			 continue;}

                //
          //return pointer; //{2} +1 to mas
      } /*else if(pattern[p]!='\0'&&text[t]=='\0'){
          p=0;
	  pointer=0;
          //show_mas();
	  //return 0;
	  break; //{3} return mas, if no pattern -- default mas[1]=={-1}
      }*/ else if(pattern[p]!='\0'&&text[t]!='\0'){                        //SEARCH CONTINUE
       p=0;
          pointer=0;
                     //
        		 if(pattern[p]==text[t]){
				 pointer=t;
				 ++p;
			 continue;}
                     //
	  //t=t-1;
         // continue;
      }
  //}
           }

show_mas(number_of_symbols);
//free(result_array());
    return 0; //{4} return mass here (if no pattern -- mas[1]=={-1})
    /* ... */
}


int Search::strstr_multi_qt(const char *text, const char *pattern)
{
    result_array()[0]=-1;
    if(strlenpp(text)==0){

        QMessageBox::about(0,"Search results", "File is empty. Nothing to search.");
        return 1;
    }

    int p=0;
    int pointer=0;
    size_t number_of_symbols = 0;

    // SEARCH :
  for(size_t t=0;t<=strlenpp(text);++t){
      if(text[t]!='\0')++number_of_symbols; // counter of bytes

      if(pattern[p]==text[t]){                                       //IF 1ST LETTER OF PATTERN IS FIND IN TEXT
          if(pointer==0&&p==0){pointer=t;}

      if(pattern[p]=='\0'&&text[t]=='\0'){
         result_array()=increase(result_array(),result_arr_size(),pointer); //PATTERN IS FIND AT THE END OF TEXT


                break;     }

          ++p;
          continue;
      } else if(pattern[p]=='\0'){
          //
          result_array()=increase(result_array(),result_arr_size(),pointer); //PATTERN FIND, AND TEXT IS CONTINUE

                 p=0;
          pointer=0;

                 if(pattern[p]==text[t]){
                 pointer=t;
                 ++p;
             continue;}


      } else if(pattern[p]!='\0'&&text[t]!='\0'){                        //SEARCH CONTINUE
       p=0;
          pointer=0;

                 if(pattern[p]==text[t]){
                 pointer=t;
                 ++p;
             continue;}

      }

           }

show_mas(number_of_symbols);

    return 0;

}











//int main(int argc, char* argv[])
//{
//Search ss{};
//std::ifstream inFile;
//inFile.open(argv[1]);

//if (inFile.fail())
//{ // file could not be opened
//do{
//std::cout << RED << "file not found/path to file not exist."<<RESET<< std::endl;
//std::cout << "Please insert correct path." << std::endl;
//	std::string path;
//	std::getline(std::cin, path);
//inFile.open(path);
//}while(inFile.fail());
//	//return 1;
//}

//if (Search::is_empty(inFile))
//{
//	// file is empty?
//	std::cout << "file is empty. Nothing to search." << std::endl;
//	return 1;
//}

//std::stringstream strStream;
//strStream << inFile.rdbuf();
//std::string str = strStream.str();
//std::cout<< "data from file to search: " << std::endl;
//std::cout << GREEN << str << RESET << "\n"; //!!!!!!!!!!!!!!!!!!)))


//std::cout << "please insert pattern for text search"<<std::endl;
//do{
//    //std::cin >> ss.pat;
//	std::getline(std::cin,ss.pat);
//}while(ss.pat[0]=='\0');

// // ->->

//char const * pat_cstr = ss.pat.c_str();
// //std::string * p_string =&str; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//const char *cstr=str.c_str(); //for literal operations "c-style"

//std::cout << "pattern exist in next literal positions(from zero): " << std::endl;
//ss.strstr_multi(cstr,pat_cstr);//ss.pat);

//ss.show_finale(str,pat_cstr);//ss.pat);
// //return clear_memory();
//return 0;
//}

