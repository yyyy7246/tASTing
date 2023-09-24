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

	buffer[fileSize] = '\0';  // Null-terminate the string

	fclose(file);   // Close the file

	return buffer;  // Return pointer to the string containing file content 
}

int main() {
	const char *str = readFile("test.json");
    json_value json = json_create(str); //json 파일 전체 불러오기
    json_value ext = json_get(json, "ext");
    printf("함수 개수 : %d\n",json_len(ext)); // 함수 개수 출력


    json_value obj = json_get(ext,0); // 0을 for문으로 변경해야함.
    json_value decl = json_get(obj, "decl");
    char *name = json_get_string(decl, "name");
    printf("함수 이름 : %s\n",name);            // 함수 이름 출력





    json_value obj2 = json_get(ext,1); // 0번째 함수는 void여서 return이 없기에 if null로 확인해야할듯
    //json_is_null 이라는 bool 타입이 있어서 이거 사용하면 됨.
    json_value body = json_get(obj2, "body");
    json_value block_items = json_get(body, "block_items");
    json_value block_items_obj = json_get(block_items,2);
    json_value expr = json_get(block_items_obj , "expr");
    json_value expr_type = json_get(expr,"type");
    printf("함수 리턴 타입 : ");
    json_print(expr_type); // 함수 리턴 타입 추출
    printf("\n");
    
    

    // json_value type = json_get(decl,"type");
    // json_value type2 = json_get(type,"type");
    // json_value type3 = json_get(type2,"type");
    // json_value names = json_get(type3,"names");
    // json_print(names); // 함수 형태 출력
    // printf("\n");

    free((void*)str);      // Free memory allocated by readFile function
    str = NULL;            // Set pointer to null after freeing memory to avoid dangling pointer 
	return 0;
}