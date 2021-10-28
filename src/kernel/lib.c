#include "lib.h"

void itoa(unsigned int n, char* buf)
{
    int i;
    if (n < 10)
    {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    itoa(n / 10, buf);
    for(i = 0; buf[i] != '\0'; i++)
    {
    }

    buf[i] = (n % 10) + '0';
    buf[i+1] = '\0';
}

int atoi(char* pstr)
{
    int int_ret = 0;
    int int_sign = 1;
    
    if (pstr == '\0')
    {
        return -1;
    }

    while (((*pstr) == ' ') ||
        ((*pstr) == '\n') ||
        ((*pstr) == '\t') ||
        ((*pstr) == '\b'))
    {
        pstr++; 
    }

    if (*pstr == '-') 
    { 
        int_sign = -1; 
    } 
    if (*pstr == '-' || *pstr == '+') 
    { 
        pstr++; 
    } 
    
    while (*pstr >= '0' && *pstr <= '9')
    {
        int_ret = int_ret * 10 + *pstr - '0'; 
        pstr++; 
    }
    int_ret = int_sign * int_ret;
    return int_ret;
} 

void xtoa(unsigned int n, char * buf)
{
    int i;
    if (n < 16)
    {
        if (n < 10)
        {
            buf[0] = n + '0';
        }
        else
        {
            buf[0] = n - 10 + 'a';
        }
        buf[1] = '\0';
        return;
    }
    xtoa(n / 16, buf);
    for (i = 0; buf[i] != '\0'; i++)
    {
    }
    if ((n % 16) < 10)
    {
        buf[i] = (n % 16) + '0';
    }
    else
    {
        buf[i] = (n % 16) - 10 + 'a';
    }
    buf[i + 1] = '\0';
}

int isdigit(unsigned char c)
{
    if (c >= '0' && c <= '9')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isletter(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return 1;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        return 1; 
    }

    return 0;
}

int toupper(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 32;
    }
    return c;
}

void* memset(void* s, int c, unsigned int count)
{
    char* xs = (char*)s;

    while (count--)
    {
        *xs++ = c;
    }
    return s;
}

char* strcpy(char* dest, const char* src)
{
    char *tmp = dest;
    while ((*dest++ = *src++) != '\0')
    {
    }
    return tmp;
}

unsigned int strlen(const char* s)
{
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc)
    {
    }
    return sc - s;
}

char* strcat(char* dest, const char* src)
{
    char *tmp = dest;

    while (*dest)
    {
        dest++;
    }

    while ((*dest++ = *src++) != '\0')
    {
    }
    return tmp;
}

// from: https://github.com/mirtoto/snprintf/blob/master/src/snprintf.c
// Copyright (C) 2019 Miroslaw Toton, mirtoto@gmail.com
struct DATA
{
    size_t counter;                         /**< counter of length of string in DATA::ps */
    size_t ps_size;                         /**< size of DATA::ps - 1 */
    char *ps;                               /**< pointer to output string */
    const char *pf;                         /**< pointer to input format string */

/** Value of DATA::width - undefined width of field. */
#define WIDTH_UNSET                    -1

    int width;                              /**< width of field */

/** Value of DATA::precision - undefined precision of field. */
#define PRECISION_UNSET            -1

    int precision;

/** Value of DATA::align - undefined align of field. */
#define ALIGN_UNSET                     0
/** Value of DATA::align - align right of field. */
#define ALIGN_RIGHT                     1
/** Value of DATA::align - align left of field. */
#define ALIGN_LEFT                      2

    unsigned int align:2;           /**< align of field */
    unsigned int is_square:1;       /**< is field with hash flag? */
    unsigned int is_space:1;        /**< is field with space flag? */
    unsigned int is_dot:1;          /**< is field with dot flag? */
    unsigned int is_star_w:1;       /**< is field with width defined? */
    unsigned int is_star_p:1;       /**< is field with precision defined? */

/** Value of DATA::a_long - "int" type of input argument. */
#define INT_LEN_DEFAULT             0
/** Value of DATA::a_long - "long" type of input argument. */
#define INT_LEN_LONG                    1
/** Value of DATA::a_long - "long long" of input type argument. */
#define INT_LEN_LONG_LONG         2
/** Value of DATA::a_long - "short" type of input argument. */
#define INT_LEN_SHORT                 3
/** Value of DATA::a_long - "char" type of input argument. */
#define INT_LEN_CHAR                    4

    unsigned int a_long:3;          /**< type of input */

    unsigned int rfu:6;             /**< RFU */

    char pad;                       /**< padding character */

    char slop[5];                   /**< RFU */
};

/** Round off to the precision. */
#define ROUND_TO_PRECISION(d, p) \
    ((d < 0.) ? d - pow_10(-(p)->precision) * 0.5 : d + pow_10(-(p)->precision) * 0.5)

/** Put a @p c character to output buffer if there is enough space. */
#define PUT_CHAR(c, p)                      \
    if ((p)->counter < (p)->ps_size) {      \
        if ((p)->ps != NULL) {              \
            *(p)->ps++ = (c);               \
        }                                   \
        (p)->counter++;                     \
    }

/** Put optionally '+' character to to output buffer if there is enough space. */
#define PUT_PLUS(d, p)                          \
    if ((d) > 0 && (p)->align == ALIGN_RIGHT) { \
        PUT_CHAR('+', p);                       \
    }

/** Put optionally ' ' character to to output buffer if there is enough space. */
#define PUT_SPACE(d, p)                 \
    if ((p)->is_space && (d) > 0) {     \
        PUT_CHAR(' ', p);               \
    }

/** Padding right optionally. */
#define PAD_RIGHT(p)                                    \
    if ((p)->width > 0 && (p)->align != ALIGN_LEFT) {   \
        for (; (p)->width > 0; (p)->width--) {          \
            PUT_CHAR((p)->pad, p);                      \
        }                                               \
    }

/** Padding left optionally. */
#define PAD_LEFT(p)                                     \
    if ((p)->width > 0 && (p)->align == ALIGN_LEFT) {   \
        for (; (p)->width > 0; (p)->width--) {          \
            PUT_CHAR((p)->pad, p);                      \
        }                                               \
    }

/** Get width and precision arguments if available. */
#define WIDTH_AND_PRECISION_ARGS(p)                     \
    if ((p)->is_star_w) {                               \
        (p)->width = va_arg(args, int);                 \
    }                                                   \
    if ((p)->is_star_p) {                               \
        (p)->precision = va_arg(args, int);             \
    }

/** Get integer argument of given type and convert it to long long. */
#define INTEGER_ARG(p, type, ll)                        \
    WIDTH_AND_PRECISION_ARGS(p);                        \
    if ((p)->a_long == INT_LEN_LONG_LONG) {             \
        ll = (long long)va_arg(args, type long long);   \
    } else if ((p)->a_long == INT_LEN_LONG) {           \
        ll = (long long)va_arg(args, type long);        \
    } else {                                            \
        type int a = va_arg(args, type int);            \
        if ((p)->a_long == INT_LEN_SHORT) {             \
            ll = (type short)a;                         \
        } else if ((p)->a_long == INT_LEN_CHAR) {       \
            ll = (type char)a;                          \
        } else {                                        \
            ll = a;                                     \
        }                                               \
    }

/** Get double argument. */
#define DOUBLE_ARG(p, d)                                \
    WIDTH_AND_PRECISION_ARGS(p);                        \
    if ((p)->precision == PRECISION_UNSET) {            \
        (p)->precision = 6;                             \
    }                                                   \
    d = va_arg(args, double);

static int strtoi(const char* a, int* res)
{
    int i = 0;
    *res = 0;

    for (; a[i] != '\0' && isdigit(a[i]); i++)
    {
        *res = *res * 10 + (a[i] - '0');
    }
    return i;
}

static void inttoa(
    long long number,
    int is_signed,
    int precision,
    int base,
    char *output,
    size_t output_size
)
{
    size_t i = 0, j;

    output_size--;
    if (number != 0)
    {
        unsigned long long n;

        if (is_signed && number < 0)
        {
            n = (unsigned long long)-number;
            output_size--;
        }
        else
        {
            n = (unsigned long long)number;
        }

        while (n != 0 && i < output_size)
        {
            int r = (int)(n % (unsigned long long)(base));
            output[i++] = (char)r + (r < 10 ? '0' : 'a' - 10);
            n /= (unsigned long long)(base);
        }

        if (precision > 0)
        {
            for (; i < (size_t)precision && i < output_size; i++)
            {
                output[i] = '0';
            }
        }

        if (is_signed && number < 0)
        {
            output[i++] = '-';
        }
        output[i] = '\0';
        
        for (i--, j = 0; j < i; j++, i--)
        {
            char tmp = output[i];
            output[i] = output[j];
            output[j] = tmp;
        }
    }
    else
    {
        precision = precision < 0 ? 1 : precision;
        for (i = 0; i < (size_t)precision && i < output_size; i++)
        {
            output[i] = '0';
        }
        output[i] = '\0';
    }
}

static double pow_10(int n)
{
    int i = 1;
    double p = 1., m;

    if (n < 0)
    {
        n = -n;
        m = .1;
    }
    else 
    {
        m = 10.;
    }

    for (; i <= n; i++)
    {
        p *= m;
    }

    return p;
}

static int log_10(double r)
{
    int i = 0;
    double result = 1.;

    if (r == 0.)
    {
        return 0;
    }
    
    if (r < 0.)
    {
        r = -r;
    }

    if (r < 1.)
    {
        for (; result >= r; i++)
        {
            result *= .1;
        }

        i = -i;
    }
    else
    {
        for (; result <= r; i++)
        {
            result *= 10.;
        }

        --i;
    }

    return i;
}

static double integral(double real, double *ip)
{
    int log;
    double real_integral = 0.;

    if (real == 0.)
    {
        *ip = 0.;
        return 0.;
    }

    if (real < 0.)
    {
        real = -real;
    }

    if (real < 1.) {
        *ip = 0.;
        return real;
    }

    for (log = log_10(real); log >= 0; log--)
    {
        double i = 0., p = pow_10(log);
        double s = (real - real_integral) / p;
        for (; i + 1. <= s; i++) {}
        real_integral += i * p;
    }

    *ip = real_integral;
    return (real - real_integral);
}

#define MAX_INTEGRAL_SIZE (99 + 1)
#define MAX_FRACTION_SIZE (29 + 1)
#define PRECISION (1.e-6)

static void floattoa(double number, int precision,
        char *output_integral, size_t output_integral_size,
        char *output_fraction, size_t output_fraction_size)
{

    size_t i, j;
    int is_negative = 0;
    double ip, fp;
    double fraction;

    if (number == 0.)
    {
        output_integral[0] = output_fraction[0] = '0';
        output_integral[1] = output_fraction[1] = '\0';
        return;
    }

    if (number < 0.)
    {
        number = -number;
        is_negative = 1;
        output_integral_size--;
    }

    fraction = integral(number, &ip);
    number = ip;
    if (ip == 0.)
    {
        output_integral[0] = '0';
        i = 1;
    }
    else
    {
        for (i = 0; i < output_integral_size - 1 && number != 0.; ++i)
        {
            number /= 10;
            output_integral[i] = (char)((integral(number, &ip) + PRECISION) * 10) + '0';
            if (!isdigit(output_integral[i]))
            {
                break;
            }
            number = ip;
        }
    }

    if (number != 0.)
    {
        for (i = 0; i < output_integral_size - 1; ++i)
        {
            output_integral[i] = '9';
        }
    }

    if (is_negative)
    {
        output_integral[i++] = '-';
    }

    output_integral[i] = '\0';

    for (i--, j = 0; j < i; j++, i--)
    {
        char tmp = output_integral[i];
        output_integral[i] = output_integral[j];
        output_integral[j] = tmp;
    }

    for (i = 0, fp = fraction; precision > 0 && i < output_fraction_size - 1; i++, precision--)
    {
        output_fraction[i] = (char)(int)((fp + PRECISION) * 10. + '0');
        if (!isdigit(output_fraction[i])) 
        {
            break;
        }

        fp = (fp * 10.0) - (double)(long)((fp + PRECISION) * 10.);
    }
    output_fraction[i] = '\0';
}

static void decimal(struct DATA *p, long long ll)
{
    char number[MAX_INTEGRAL_SIZE], *pnumber = number;
    inttoa(ll,
        *p->pf == 'i' || *p->pf == 'd', 
        p->precision, 10,
        number,
        sizeof(number)
    );

    p->width -= strlen(number);
    PAD_RIGHT(p);

    PUT_PLUS(ll, p);
    PUT_SPACE(ll, p);

    for (; *pnumber != '\0'; pnumber++)
    {
        PUT_CHAR(*pnumber, p);
    }

    PAD_LEFT(p);
}

static void octal(struct DATA *p, long long ll)
{
    char number[MAX_INTEGRAL_SIZE], *pnumber = number;
    inttoa(ll, 0, p->precision, 8, number, sizeof(number));

    p->width -= strlen(number);
    PAD_RIGHT(p);

    if (p->is_square && *number != '\0')
    {
        PUT_CHAR('0', p);
    }

    for (; *pnumber != '\0'; pnumber++)
    {
        PUT_CHAR(*pnumber, p);
    }

    PAD_LEFT(p);
}

static void hex(struct DATA *p, long long ll)
{
    char number[MAX_INTEGRAL_SIZE], *pnumber = number;
    inttoa(ll, 0, p->precision, 16, number, sizeof(number));

    p->width -= strlen(number);
    PAD_RIGHT(p);

    if (p->is_square && *number != '\0')
    {
        PUT_CHAR('0', p);
        PUT_CHAR(*p->pf == 'p' ? 'x' : *p->pf, p);
    }

    for (; *pnumber != '\0'; pnumber++)
    {
        PUT_CHAR((*p->pf == 'X' ? (char)toupper(*pnumber) : *pnumber), p);
    }

    PAD_LEFT(p);
}

static void strings(struct DATA *p, char *s)
{
    int len = (int)strlen(s);
    if (p->precision != PRECISION_UNSET && len > p->precision)
    {
        len = p->precision;
    }

    p->width -= len;

    PAD_RIGHT(p);

    for (; len-- > 0; s++)
    {
        PUT_CHAR(*s, p);
    }

    PAD_LEFT(p);
}

static void floating(struct DATA *p, double d)
{
    char integral[MAX_INTEGRAL_SIZE], *pintegral = integral;
    char fraction[MAX_FRACTION_SIZE], *pfraction = fraction;

    d = ROUND_TO_PRECISION(d, p);
    floattoa(d, p->precision,
        integral, sizeof(integral), fraction, sizeof(fraction));
        
    if (d > 0. && p->align == ALIGN_RIGHT)
    {
        p->width -= 1;    
    }
    p->width -= p->is_space + (int)strlen(integral) + p->precision + 1;
    if (p->precision == 0)
    {
        p->width += 1;
    }
    
    PAD_RIGHT(p);
    PUT_PLUS(d, p);
    PUT_SPACE(d, p);

    for (; *pintegral != '\0'; pintegral++)
    {
        PUT_CHAR(*pintegral, p);
    }

    if (p->precision != 0 || p->is_square)
    {
        PUT_CHAR('.', p);
    }

    if (*p->pf == 'g' || *p->pf == 'G')
    {
        size_t i;
        for (i = strlen(fraction); i > 0 && fraction[i - 1] == '0'; i--)
        {
            fraction[i - 1] = '\0';
        }
    }

    for (; *pfraction != '\0'; pfraction++)
    {
        PUT_CHAR(*pfraction, p);
    }

    PAD_LEFT(p);
}

static void exponent(struct DATA *p, double d)
{
    char integral[MAX_INTEGRAL_SIZE], *pintegral = integral;
    char fraction[MAX_FRACTION_SIZE], *pfraction = fraction;
    int log = log_10(d);
    d /= pow_10(log);
    d = ROUND_TO_PRECISION(d, p);

    floattoa(d, p->precision,
        integral, sizeof(integral), fraction, sizeof(fraction));

    if (d > 0. && p->align == ALIGN_RIGHT)
    {
        p->width -= 1;    
    }
    p->width -= p->is_space + p->precision + 7;
    
    PAD_RIGHT(p);
    PUT_PLUS(d, p);
    PUT_SPACE(d, p);

    for (; *pintegral != '\0'; pintegral++)
    {
        PUT_CHAR(*pintegral, p);
    }

    if (p->precision != 0 || p->is_square)
    {
        PUT_CHAR('.', p);
    }

    if (*p->pf == 'g' || *p->pf == 'G')
    {
        size_t i;
        for (i = strlen(fraction); i > 0 && fraction[i - 1] == '0'; i--)
        {
            fraction[i - 1] = '\0';
        }
    }
    for (; *pfraction != '\0'; pfraction++)
    {
        PUT_CHAR(*pfraction, p);
    }

    if (*p->pf == 'g' || *p->pf == 'e')
    {
        PUT_CHAR('e', p);
    }
    else
    {
        PUT_CHAR('E', p);
    }

    if (log >= 0)
    {
        PUT_CHAR('+', p);
    }

    inttoa(log, 1, 2, 10, integral, sizeof(integral));
    for (pintegral = integral; *pintegral != '\0'; pintegral++)
    {
        PUT_CHAR(*pintegral, p);
    }

    PAD_LEFT(p);
}

static void conv_flags(struct DATA *p)
{
    p->width = WIDTH_UNSET;
    p->precision = PRECISION_UNSET;
    p->is_star_w = p->is_star_p = 0;
    p->is_square = p->is_space = 0;
    p->a_long = INT_LEN_LONG_LONG;
    p->align = ALIGN_UNSET;
    p->pad = ' ';
    p->is_dot = 0;

    for (; p != NULL && p->pf != NULL; p->pf++)
    {
        switch (*p->pf)
        {
            case ' ':
                p->is_space = 1;
                break;

            case '#':
                p->is_square = 1;
                break;

            case '*':
                if (p->width == WIDTH_UNSET)
                {
                    p->width = 1;
                    p->is_star_w = 1;
                }
                else
                {
                    p->precision = 1;
                    p->is_star_p = 1;
                }
                break;

            case '+':
                p->align = ALIGN_RIGHT;
                break;

            case '-':
                p->align = ALIGN_LEFT;
                break;

            case '.':
                if (p->width == WIDTH_UNSET)
                {
                    p->width = 0;
                }
                p->is_dot = 1;
                break;

            case '0':
                p->pad = '0';
                if (p->is_dot)
                {
                    p->precision = 0;
                }
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                p->pf += strtoi(p->pf,
                    p->width == WIDTH_UNSET ? &p->width : &p->precision) - 1;
                break;

            case '%':
                return;

            default:
                p->pf--;
                return;
        }
    }
}

int vsnprintf(char *string, size_t length, const char *format, va_list args)
{
    struct DATA data;
    if (string == NULL || length < 1)
    {
        return -1;
    }

    data.ps_size = length - 1;
    data.ps = string;
    data.pf = format;
    data.counter = 0;

    for (; *data.pf != '\0' && (data.counter < data.ps_size); data.pf++)
    {
        if (*data.pf == '%')
        {
            int is_continue = 1;
            conv_flags(&data);
            while (*data.pf != '\0' && is_continue)
            {
                switch (*(++data.pf))
                {
                    case '\0':
                    {
                        PUT_CHAR('%', &data);
                        if (data.ps != NULL)
                        {
                            *data.ps = '\0';
                        }
                        return (int)data.counter;
                    }

                    case 'f':
                    case 'F':
                    {
                        double d;
                        DOUBLE_ARG(&data, d);
                        floating(&data, d);
                        is_continue = 0;
                        break;
                    }

                    case 'e':
                    case 'E':
                    {
                        double d;
                        DOUBLE_ARG(&data, d);
                        exponent(&data, d);
                        is_continue = 0;
                        break;
                    }

                    case 'g':
                    case 'G':
                    {
                        int log;
                        double d;
                        DOUBLE_ARG(&data, d);
                        log = log_10(d);
                        if (-4 < log && log < data.precision)
                        {
                            floating(&data, d);
                        }
                        else
                        {
                            exponent(&data, d);
                        }
                        is_continue = 0;
                        break;
                    }

                    case 'u':
                    {
                        long long ll;
                        INTEGER_ARG(&data, unsigned, ll);
                        decimal(&data, ll);
                        is_continue = 0;
                        break;
                    }

                    case 'i':
                    case 'd':
                    {
                        long long ll;
                        INTEGER_ARG(&data, signed, ll);
                        decimal(&data, ll);
                        is_continue = 0;
                        break;
                    }

                    case 'o':
                    {
                        long long ll;
                        INTEGER_ARG(&data, unsigned, ll);
                        octal(&data, ll);
                        is_continue = 0;
                        break;
                    }

                    case 'x':
                    case 'X':
                    {
                        long long ll;
                        INTEGER_ARG(&data, unsigned, ll);
                        hex(&data, ll);
                        is_continue = 0;
                        break;
                    }

                    case 'c':
                    {
                        int i = va_arg(args, int);
                        PUT_CHAR((char)i, &data);
                        is_continue = 0;
                        break;
                    }

                    case 's':
                    {
                        WIDTH_AND_PRECISION_ARGS(&data);
                        strings(&data, va_arg(args, char *));
                        is_continue = 0;
                        break;
                    }
                    case 'p':
                    {
                        void *v = va_arg(args, void *);
                        data.is_square = 1;
                        if (v == NULL)
                        {
                            strings(&data, "(nil)");
                        } 
                        else
                        {
                            hex(&data, (long long)v);
                        }
                        is_continue = 0;
                        break;
                    }

                    case 'n':
                    {
                        *(va_arg(args, int *)) = (int)data.counter;
                        is_continue = 0;
                        break;
                    }

                    case 'l':
                        if (data.a_long == INT_LEN_LONG)
                        {
                            data.a_long = INT_LEN_LONG_LONG;
                        }
                        else
                        {
                            data.a_long = INT_LEN_LONG;
                        }
                        break;

                    case 'h':
                        if (data.a_long == INT_LEN_SHORT)
                        {
                            data.a_long = INT_LEN_CHAR;
                        }
                        else
                        {
                            data.a_long = INT_LEN_SHORT;
                        }
                        break;

                    case '%': /* nothing just % */
                        PUT_CHAR('%', &data);
                        is_continue = 0;
                        break;

                    case '#':
                    case ' ':
                    case '+':
                    case '*':
                    case '-':
                    case '.':
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        conv_flags(&data);
                        break;

                    default:
                        PUT_CHAR('%', &data);
                        is_continue = 0;
                        break;
                }
            }
        }
        else
        {
            PUT_CHAR(*data.pf, &data);
        }
    }

    if (data.ps != NULL)
    {
        *data.ps = '\0';
    }
    return (int)data.counter;
}

int snprintf(char *string, size_t length, const char *format, ...)
{
    int ret;

    va_list args;
    va_start(args, format);
    ret = vsnprintf(string, length, format, args);
    va_end(args);

    return ret;
}