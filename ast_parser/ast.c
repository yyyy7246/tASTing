#include <stdio.h>
#include <stdlib.h>
#include "json_c.c"

char* readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char*)malloc(sizeof(char) * (fileSize + 1));
    

	size_t result = fread(buffer, sizeof(char), fileSize, file);

	if(result != fileSize){
		printf("Reading error.\n");
		free(buffer);
		return NULL;
	}

	buffer[fileSize] = '\0';  

	fclose(file);   

	return buffer;  
}



void search_name_names(json_value node){
    json_value type = json_get(node,"type"); 
    json_value names = json_get(type, "names");
    char *name = json_get_string(type, "name");
    char *tmp = json_get_string(type,"tmptmptmptmptmptmp");
    if (json_len(names) == 0 && name == tmp ){             
        search_name_names(type);
    }else{
        if(json_len(names) ==0){
            printf("%s\n",name);
        } else{
            json_print(names);
            printf("\n");
        }
    }
}

void count_if_def(json_value node, int *if_count) {                
    if (strcmp("If",json_get_string(node,"_nodetype")) == 0){
        *if_count+=1;
        json_value iffalse = json_get(node,3); //iffalse는 4번째에 위치함
        if (json_len(iffalse) != 0){
            count_if_def(iffalse,if_count);
            json_value iffalse_items = json_get(iffalse,"block_items");
            if (json_len(iffalse_items) !=0 ){
                for (int i=0; i<json_len(iffalse_items); i++){
                    json_value iffalse_item = json_get(iffalse_items,i);
                    count_if_def(iffalse_item,if_count);
                }
            }
        }

        json_value iftrue = json_get(node,4); //iffalse는 5번째에 위치함
        count_if_def(iftrue,if_count);
        json_value iftrue_items = json_get(iftrue,"block_items");
        if (json_len(iftrue_items) !=0 ){
                for (int i=0; i<json_len(iftrue_items); i++){
                    json_value iftrue_item = json_get(iftrue_items,i);
                    count_if_def(iftrue_item,if_count);
                }
            }
    
    }
    else if(strcmp("While",json_get_string(node,"_nodetype")) == 0){
        json_value stmt = json_get(node,"stmt");
        json_value stmt_block_items = json_get(stmt,"block_items");
        for (int i=0; i<json_len(stmt_block_items); i++){
            json_value stmt_item = json_get(stmt_block_items,i);
            count_if_def(stmt_item,if_count);
        }
    }

}
int count_func_def(json_value node, int *func_count){
    for (int i=0; i<json_len(node); i++){
        json_value obj = json_get(node,i);
        char *nodetype = json_get_string(obj,"_nodetype");
        if (strcmp("FuncDef",nodetype) == 0)
        {
            *func_count +=1;
        }
    }
    return func_count;
}



void print_returntype_def(json_value node, int *func_count){ 
    printf("%d번재 리턴 타입 : ",*func_count);
    search_name_names(node);
}


void print_func_name(json_value node, int *func_count){
    char *name = json_get_string(node, "name");
    printf("%d번째 함수 이름 : %s\n",*func_count,name);
}


void print_params_info(json_value node, int *func_count){
    json_value type = json_get(node, "type");
    json_value args = json_get(type,"args");
    if (json_len(args)==0){
        printf("%d번째 함수의 파라미터는 없음.\n",*func_count);
    }
    json_value params = json_get(args,"params");
    for (int i=0; i<json_len(params); i++){
        json_value param = json_get(params,i);    
        char *name = json_get_string(param,"name");   
        printf("%d번째 함수의 %d번째 파라미터 변수 명 : %s, 타입 : ",*func_count,i+1,name);
        search_name_names(param);

    }
}




int main() {
	const char *str = readFile("btree.json");
    int total_func_count = 0;
    json_value json = json_create(str); 
    json_value ext = json_get(json, "ext");
    count_func_def(ext, &total_func_count);
    printf("함수 개수: %d\n",total_func_count);

    int func_count = 0;
    int total_if_count = 0;
    for (int i=0; i<json_len(ext); i++){
        json_value obj = json_get(ext,i);
        char *nodetype = json_get_string(obj,"_nodetype");
        if (strcmp("FuncDef",nodetype) == 0){ 
            func_count +=1;
            json_value decl = json_get(obj, "decl");
            
            print_returntype_def(decl,&func_count); // 함수 리턴 타입 추출
            
 
            print_func_name(decl,&func_count); // 함수 이름 추출


            print_params_info(decl,&func_count); // 함수 파라미터 타입, 변수명 추출


            int if_count = 0; // 함수 if조건 개수 추출
            json_value body = json_get(obj, "body"); 
            json_value block_items = json_get(body, "block_items"); 
            for (int i=0; i<json_len(block_items); i++){
                json_value if_item = json_get(block_items,i); 
                count_if_def(if_item, &if_count);
            }
            printf("%d번째 함수 if 개수 : %d\n\n",func_count,if_count);
            total_if_count = total_if_count + if_count;
        }
    } 
    printf("모든 if의 개수 : %d\n", total_if_count);


    

    free((void*)str);      
    str = NULL;            
	return 0;
}