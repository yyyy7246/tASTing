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

int main() {
	const char *str = readFile("btree.json");
    int func_count = 0;
    json_value json = json_create(str); 
    json_value ext = json_get(json, "ext");
    for (int i=0; i<json_len(ext); i++){
        json_value obj = json_get(ext,i);
        char *nodetype = json_get_string(obj,"_nodetype");
        if (strcmp("FuncDef",nodetype) == 0)
        {
            func_count +=1;
        }
    }
    printf("함수 개수: %d\n",func_count);

    func_count = 0;
    for (int i=0; i<json_len(ext); i++){
        json_value obj = json_get(ext,i);
        char *nodetype = json_get_string(obj,"_nodetype");
        if (strcmp("FuncDef",nodetype) == 0){ 
            func_count +=1;
            json_value decl = json_get(obj, "decl");
            json_value type1 = json_get(decl, "type");
            json_value type2 = json_get(type1, "type");
            json_value type3 = json_get(type2, "type");
            json_value names = json_get(type3, "names");
            if (json_len(names) == 0){
                json_value name = json_get(type3,"name");
                if (json_len(name) == 0){
                    json_value type4 = json_get(type3,"type");
                    json_value names2 = json_get(type4,"names");
                    if (json_len(names2) == 0){
                        json_value name = json_get(type4,"name");
                        printf("%d번재 리턴 타입 : ",func_count);
                        json_print(name);
                        printf("\n");
                    }
                    else{
                        printf("%d번재 리턴 타입 : ",func_count);
                        json_print(names2);
                        printf("\n");
                    }
                }else{
                    printf("%d번재 리턴 타입 : ",func_count);
                    json_print(name);
                    printf("\n");
                }
            }
            else{
                printf("%d번재 리턴 타입 : ",func_count);
                json_print(names);
                printf("\n");
            }
            char *name = json_get_string(decl, "name");
            printf("%d번째 함수 이름 : %s\n",func_count,name);
        }
    }            



    int if_count = 0;
    for (int i=0; i<json_len(ext); i++){
        json_value obj = json_get(ext,i);
        char *nodetype = json_get_string(obj,"_nodetype");
        if (strcmp("FuncDef",nodetype) == 0){
            json_value body = json_get(obj, "body"); 
            json_value block_items = json_get(body, "block_items");
            for (int i=0; i<json_len(block_items); i++){
                json_value if_item = json_get(block_items,i); 
                count_if_def(if_item, &if_count);
            }
        }
    }
    printf("count : %d\n",if_count);

    free((void*)str);      
    str = NULL;            
	return 0;
}