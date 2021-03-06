 /**
 * @file message_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Messages handler header (all from scratch) for NES416/HW1
 * @date 2021-03-18
 *
 */


/* Macro to make the function of get values from client message */
#define var1(which,var1,var2)  ((which) == (0) ? (var1) : (var2))
#define var2(which,var1,var2)  ((which%2) == (0) ? (var1) : (var2))

/* Enumeration values to make the operation more understandable */
//enum operations {ERR, ADD, MINUS, MUL, DIV, GPA, EXIT};

/* Constants values to make the operation more understandable */
#define ERR 0
#define ADD 1
#define MINUS 2
#define MUL 3
#define DIV 4
#define GPA 5
#define EXIT 6
/**
 * @brief Just To make the code clearer, I separated this part from the message handler.
 *  It's responssible to determine if this character represents an operation ( + , - , * , / ).
 * 
 * @param op 
 * @return enum operations 
 */
int decode_operation(char op)   {
    int ret = ERR;
    switch (op) {
        case '+':   ret = ADD;  break;
        case '-':   ret = MINUS;    break;
        case '*':   ret = MUL;  break;
        case '/':   ret = DIV;  break;
        default:
            ret = ERR;
    }
    return ret;
}

/**
 * @brief This function is isolated from main() to keep the networking part clearer. It's
 *  responsible to handle the function of understanding the client's message, and
 *  detemine if it's valid or not.
 * 
 * @param msg expression user sent and have a valid choice (ADD || SUBSTRACT || MULTIPLY || DIVISION).
 * @param len expression length.
 * @param result pointer to an integer to pass the expression result in.
 * @param choice client choice, should be compared with the operation in the expression.
 * @return int. If the expression is valid it will return 0, and the result will be passed to 
 *  the variable pointed by argument result.
 */
int handle_msg(char msg[], int len, float *result, int choice)
{
    int x = 0, y = 0, res = 0;
    int op = ERR;

    /*
     *  If a == 0:
     *      we are filling variable x
     *  
     *  else if a == 1:
     *      we are filling variable y
     *
     *  else:
     *      We have more than one oprator, so return error.
     */
    unsigned char a = 0;
    char temp;

    for (int i = 0; i < len; i++)   {
        temp = msg[i];
        if (temp == ' ')    {continue;}

        /* If it's a number add it as digit to the current varibale (x or y) */
        if (temp >= '0' && temp <= '9')   {
            /* var() -> is a macro determine which variable we are filling (x or y) */
            *(var1(a, &x, &y)) += (int)temp - 48;
            *(var1(a, &x, &y)) *= 10;
        }

        else  {
            /* if it's an operator, we should turn a to > 1, so for the next iterations
                we could fill the y as the second parametr instead of x. */
            a++;
            /* If there's more than one operator return an error. */
            if (a > 1)  {return -1;}
            /* If the operator is the last or fisrt character in string, this means that we had 
                only one value and an operator, so return error. */
            else if (i == len-1 || i == 0)    {return -1;}

            op = decode_operation(temp);
            if (op == ERR || op != choice)    {return -4;}
        }
    }

    x /= 10; y /= 10;
    switch (op) {
        case ADD:   res = x + y;    break;
        case MINUS: res = x - y;    break;
        case MUL:   res = x * y;    break;
        case DIV:
            if (y == 0)
                return -3;
            res = x / y;
            break;
        default:
            return -1;
    }

    *result = res;
    return 0;
}

/**
 * @brief This function is isolated from main() to keep the networking part clearer. It's
 *  responsible to handle the Marks and Hours list sent from client which is choice 5 "GPA", calcualte
 *  it's value, check if it's valid and return the result with a state code. 
 *  The list of marks(m) and hours(h), should be like that [m,h,m,h,m,h,...m,h,-1], where -1 means
 *  "End Of List". 
 *  Marks range(0-100) / Hours range(1-3)
 * 
 * @param msg Marks and hours list received from client.
 * @param result GPA result of the list sent.
 * @param length list string length.
 * @return int. If the expression is valid it will return 0, and the result will be passed to 
 *  the variable pointed by argument result.
 */
int handle_gpa(char *msg, float *result, int length)
{
    int v=0, rv=0, m=0, h=0, full_mark=0, commas=0, res=0;
    char eof[2];

    /*  end variable will be set to (0xff) hexa value when find the "END OF LIST" (-1).
    *   So, if there's any thing other then spaces after it, we could catch it and return an error (-5) */
    unsigned char end = 0x00;

    /*  In this loop we only care about number and commas   */
    for (int i = 0; i < length-1; i++)   {
        if (msg[i] == ' ')  {continue;}
        else if (msg[i] >= '0' && msg[i] <='9')  {
            /*  Use the macro to determine where to put the value    */
            *(var2(commas, &m, &h)) += (int)msg[i] - 48;
            *(var2(commas, &m, &h)) *= 10;
        }

        /*  When we find a comma, we should check, if we had filled 'm' and 'h' variables to use them or not    */
        else if (msg[i] == ',' || msg[i] == (char)44)   {
//            *(var2(commas, &m, &h)) /= 10;
//            commas++;
            if ((commas%2) != 0)    {
                m /= 10;    h /= 10;
                v = m * h;
                res += v;
                full_mark += (100 * h);
                /* Use this for debugging */
                /*printf("\nm = %d, h = %d, v = %d, res = %d, full_mark = %d\n", m, h, v, res, full_mark);*/
                v = 0; m = 0; h = 0;
            }
            commas++;
        }

        /*  check if we reached the "END OF LIST" (-1)  */
        else if (msg[i] == '-' && msg[i+1] == '1')
            {break;}

        else    {
            rv = -5;
            break;
        }
    }

    float f = res;
    *result = (float)res / (commas+1);
    return rv;
}
