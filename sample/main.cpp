#include "../package/prengine.h"
#include <stdio.h>
#include <string>
using namespace std;
int main(int argc, char const *argv[]) {
  //输入的路由地址
  const char *input1 = "http://taofen8.com/home";
  const char *input2 = "custom-scheme://taofen8.com/detail?goodsId=12345";
  //读取config文件
  FILE *fp = fopen( "sample.conf" , "r" );
  fseek(fp , 0L , SEEK_END);
  long size = ftell(fp);
  rewind(fp);
  char *buffer = new char[size];
  fread(buffer , 1 , size , fp);
  buffer[size] = '\0';

  PRFILE* prfile = pr_creat(buffer);
  PRREWRITE *rewrite = pr_rewrite_matched_creat(prfile, input2);

  if (rewrite)
      printf("rewrite output %s.\n",pr_getoutval(rewrite));
  else
     printf("no matched.\n");

  delete[] buffer;

  return 0;
}
