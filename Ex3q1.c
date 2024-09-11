#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 120

typedef struct {//struct that i made to be a polynom 
    int degree ;
    int* coefficients;
} Polynom;

bool checkSubstring(const char* string, const char* substring);
Polynom addPolynom(Polynom p1, Polynom p2);
Polynom subtractPolynom(Polynom p1, Polynom p2);
Polynom multiplyPolynom(Polynom p1, Polynom p2);
void printPolynom(Polynom p);
void separateStrings(const char* input, char* firstString, char* secondString);
void strtokString(char *input_string, char **strings, int max_strings);
Polynom PolynomCalculate(Polynom p1, Polynom p2,Polynom (*callback)(Polynom,Polynom));


int main()
{
    char* user_in = "\0";
    size_t buffer_size = 0;
    Polynom p1;//for the first string   
    Polynom p2;//for the second
    Polynom result;

    while (1) {
        printf("Enter a two polynoms and between of them one of (ADD,SUB,MUL): ");
        // Read the user input
        size_t in_length = getline(&user_in, &buffer_size, stdin);
        // Remove the newline character
        user_in[strcspn(user_in, "\n")] = '\0';
        

        if (strcmp(user_in, "END") == 0) {
            break;
        }
        else if (in_length > MAX_INPUT_LENGTH + 1) {
            printf("Input exceeds maximum length of %d characters.\n", MAX_INPUT_LENGTH);
        }
         else {
        char firstString[MAX_INPUT_LENGTH]="\0";
        char secondString[MAX_INPUT_LENGTH]="\0";
        char *result1[100];
        char *result2[100];
        char temp[MAX_INPUT_LENGTH]="\0";
        strcpy(temp,user_in);
        separateStrings(temp, firstString, secondString);
        strtokString(firstString, result1,100);
        strtokString(secondString, result2,100);
        p1.degree = atoi(result1[0]);
        p2.degree = atoi(result2[0]);
        p1.coefficients = (int*)malloc((atoi(result1[0])+1) * sizeof(int));
        int j = p1.degree;
        for (int i = 1; i<=(p1.degree+1); i++) {//to take the coefficients of the first polynom
            p1.coefficients[j] = atoi(result1[i]);
            j--;
        }
        j = p2.degree;
        p2.coefficients = (int*)malloc((atoi(result2[0])+1) * sizeof(int));
        for (int i = 1; i<=(p2.degree+1); i++) {//to take the coefficients of the second polynom
            p2.coefficients[j] = atoi(result2[i]);
            j--;
        }

            if (checkSubstring(user_in, "ADD"))
            {
             result = PolynomCalculate(p1,p2,addPolynom);
             printPolynom(result);
            }
            else if (checkSubstring(user_in, "SUB"))
            {
            result = PolynomCalculate(p1,p2,subtractPolynom);
             printPolynom(result);
            }
            else if (checkSubstring(user_in, "MUL"))
            {
             result = PolynomCalculate(p1,p2,multiplyPolynom);
             printPolynom(result);
            }
            else{
                perror("not valid action");
            }
        }
    
    free(p1.coefficients);
    free(p2.coefficients);
    free(result.coefficients);
        
    }
   
    free(user_in);
    return 0;
}

//to check if the string have sub strings of "ADD","SUB","MUL"
bool checkSubstring(const char* string, const char* substring) {
    if (string == NULL || substring == NULL) {
        return false; // Invalid input
    }

    size_t stringLength = strlen(string);
    size_t substringLength = strlen(substring);

    if (substringLength > stringLength) {
        return false; // Substring is longer than the string
    }

    for (size_t i = 0; i <= stringLength - substringLength; i++) {
        if (strncmp(string + i, substring, substringLength) == 0) {
            return true; // Substring found
        }
    }

    return false; // Substring not found
}

Polynom addPolynom(Polynom p1, Polynom p2) {//add of polynom 
    int maxDegree = (p1.degree > p2.degree) ? p1.degree : p2.degree;
    int minDegree = (p1.degree < p2.degree) ? p1.degree : p2.degree;

    Polynom result;
    result.degree = maxDegree;
    result.coefficients = (int*)malloc((maxDegree + 1) * sizeof(int));

    int i;
    for (i = 0; i <= minDegree; i++) {
        result.coefficients[i] = p1.coefficients[i] + p2.coefficients[i];
    }

    if (p1.degree > p2.degree) {
        for (; i <= maxDegree; i++) {
            result.coefficients[i] = p1.coefficients[i];
        }
    } else {
        for (; i <= maxDegree; i++) {
            result.coefficients[i] = p2.coefficients[i];
        }
    }

    return result;
}

Polynom subtractPolynom(Polynom p1, Polynom p2) {//subtract of polynom 
    int maxDegree = (p1.degree > p2.degree) ? p1.degree : p2.degree;
    int minDegree = (p1.degree < p2.degree) ? p1.degree : p2.degree;

    Polynom result;
    result.degree = maxDegree;
    result.coefficients = (int*)malloc((maxDegree + 1) * sizeof(int));

    int i;
    for (i = 0; i <= minDegree; i++) {
        result.coefficients[i] = p1.coefficients[i] - p2.coefficients[i];
    }

    if (p1.degree > p2.degree) {
        for (; i <= maxDegree; i++) {
            result.coefficients[i] = p1.coefficients[i];
        }
    } else {
        for (; i <= maxDegree; i++) {
            result.coefficients[i] = -p2.coefficients[i];
        }
    }

    return result;
}

Polynom multiplyPolynom(Polynom p1, Polynom p2) {//multiply of polynom
    int resultDegree = p1.degree + p2.degree;
    Polynom result;
    result.degree = resultDegree;
    result.coefficients = (int*)calloc((resultDegree + 1), sizeof(int));

    int i, j;
    for (i = 0; i <= p1.degree; i++) {
        for (j = 0; j <= p2.degree; j++) {
            result.coefficients[i + j] += p1.coefficients[i] * p2.coefficients[j];
        }
    }

    return result;
}


void printPolynom(Polynom p) {//print polynoms
    int hasTerms = 0;
    for (int i = p.degree; i >= 0; i--) {
        if (p.coefficients[i] != 0) {
             hasTerms = 1;
            if (i == p.degree) {
                if (i==1)
                {
                  printf("%dx", p.coefficients[i]);
                }
                else
                     printf("%dx^%d", p.coefficients[i], i);
               
            } else if (i == 0) {
                if (p.coefficients[i] < 0) {
                    printf(" - %d", -p.coefficients[i]);
                } else {
                    printf(" + %d", p.coefficients[i]);
                }
            } else {
                if (p.coefficients[i] < 0) {
                    if (i!=1)
                    {
                        printf(" - %dx^%d", -p.coefficients[i], i);
                    }
                    else{
                        printf(" - %dx", -p.coefficients[i]);
                    }
                } else {
                       if (i!=1)
                    {
                        printf(" + %dx^%d", p.coefficients[i], i);
                    }
                    else{
                        printf(" + %dx", p.coefficients[i]);
                    }
                    
                }
            }
        }
    }
    if (!hasTerms) {
        printf("0");
    }
    printf("\n");
}


void separateStrings(const char* input, char* firstString, char* secondString) {//to get the string between the parentheses in our case ther is two 
    int i = 0;
    int len = strlen(input);

    // Find the first string enclosed in parentheses
    while (i < len && input[i] != '(') {
        i++;
    }
    if (input[i] == '(') {
        int j = 0;
        i++;
        while (i < len && input[i] != ')') {
            firstString[j] = input[i];
            i++;
            j++;
        }
        firstString[j] = '\0';
    }

    // Find the second string after the first string
    while (i < len && (input[i] != '(' || input[i] == ')')) {
        i++;
    }
    if (input[i] == '(') {
        int j = 0;
        i++;
        while (i < len && input[i] != ')') {
            secondString[j] = input[i];
            i++;
            j++;
        }
        secondString[j] = '\0';
    }
}

void strtokString(char *input_string, char **strings, int max_strings) {//strtok on (:)and (,)
    char *token;
    int i = 0;
    
    token = strtok(input_string, ":");
    while (token != NULL && i < max_strings) {
        strings[i++] = token;
        token = strtok(NULL, ",");
    }
   
}

Polynom PolynomCalculate(Polynom p1, Polynom p2,Polynom (*callback)(Polynom,Polynom)){//callback function 
    return callback(p1,p2);
}

