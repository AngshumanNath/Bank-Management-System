#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TXN      100
#define INITIAL_BAL   10000.0f

typedef struct {
    char  desc[60];
    float amount;
    float balAfter;
    char  type[4];
} Transaction;

typedef struct {
    Transaction data[MAX_TXN];
    int top;
} Stack;

void stackInit (Stack *s)                { s->top = -1; }
int  stackFull (Stack *s)                { return s->top == MAX_TXN - 1; }
int  stackEmpty(Stack *s)                { return s->top == -1; }
void stackPush (Stack *s, Transaction t) { if (!stackFull(s)) s->data[++(s->top)] = t; }

typedef struct Account {
    char   username[30];
    char   password[10];
    float  balance;
    Stack  history;
    struct Account *next;
} Account;

Account *head        = NULL;
Account *currentUser = NULL;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    printf("\n  Press ENTER to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

void printLine(char ch, int n) {
    for (int i = 0; i < n; i++) putchar(ch);
    putchar('\n');
}

void printHeader(const char *title) {
    printf("\n");
    printLine('=', 62);
    printf("  BANK MANAGEMENT SYSTEM  >>  %s\n", title);
    printf("  East West University | CSE207 Data Structures\n");
    printLine('=', 62);
}

void recordTxn(Account *acc, const char *desc, float amount, const char *type) {
    Transaction t;
    strncpy(t.desc, desc, 59);
    t.amount   = amount;
    t.balAfter = acc->balance;
    strncpy(t.type, type, 3);
    stackPush(&acc->history, t);
}

void insertAccount(const char *uname, const char *pass) {
    Account *node = (Account *)malloc(sizeof(Account));
    if (!node) { printf("Memory allocation failed!\n"); exit(1); }
    strncpy(node->username, uname, 29);
    strncpy(node->password, pass,   9);
    node->balance = INITIAL_BAL;
    stackInit(&node->history);
    node->next = NULL;

    if (!head) {
        head = node;
    } else {
        Account *tmp = head;
        while (tmp->next) tmp = tmp->next;
        tmp->next = node;
    }
}

Account *findAccount(const char *uname) {
    Account *tmp = head;
    while (tmp) {
        if (strcmp(tmp->username, uname) == 0) return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

float compoundInterest(float principal, float rate, int years) {
    if (years == 0) return principal;
    return compoundInterest(principal * (1.0f + rate / 100.0f), rate, years - 1);
}

void initMembers() {
    insertAccount("Sourav", "249");
    insertAccount("Haniya", "637");
    insertAccount("Angshu", "671");
    insertAccount("Faiza",  "483");
    insertAccount("Random", "123");
}

/* ── A. LOGIN ── */
int loginInterface() {
    char user[30], pass[10];
    clearScreen();
    printHeader("LOGIN");
    printf("\n  Username (your nickname)        : ");
    scanf("%29s", user);
    printf("  Password (last 3 digits of ID)  : ");
    scanf("%9s",  pass);

    Account *found = findAccount(user);
    if (found && strcmp(found->password, pass) == 0) {
        currentUser = found;
        printf("\n  >> Login successful! Welcome, %s.\n", user);
        pressEnter();
        return 1;
    }
    printf("\n  >> Invalid username or password!\n");
    pressEnter();
    return 0;
}

/* ── B. BALANCE CHECK ── */
void checkBalance() {
    clearScreen();
    printHeader("BALANCE CHECK");
    printf("\n  Account Holder   : %s\n",     currentUser->username);
    printf("  Current Balance  : %.2f TK\n", currentUser->balance);
    printf("\n  (All accounts start with %.2f TK)\n", INITIAL_BAL);
    pressEnter();
}

/* ── C. STATEMENT PRINT ── */
void printStatement() {
    clearScreen();
    printHeader("ACCOUNT STATEMENT");

    Stack *s = &currentUser->history;
    if (stackEmpty(s)) {
        printf("\n  No transactions found.\n");
        pressEnter();
        return;
    }

    printf("\n  %-4s  %-30s  %-4s  %-12s  %-12s\n",
           "No.", "Description", "Type", "Amount(TK)", "Balance(TK)");
    printLine('-', 68);

    for (int i = 0; i <= s->top; i++) {
        printf("  %-4d  %-30s  %-4s  %-12.2f  %-12.2f\n",
               i + 1,
               s->data[i].desc,
               s->data[i].type,
               s->data[i].amount,
               s->data[i].balAfter);
    }
    printLine('-', 68);
    printf("  Total transactions : %d\n", s->top + 1);
    printf("  Current Balance    : %.2f TK\n", currentUser->balance);
    pressEnter();
}

/* ── D. DEPOSIT ── */
void depositMoney() {
    float amount;
    clearScreen();
    printHeader("DEPOSIT");
    printf("\n  Current Balance  : %.2f TK\n", currentUser->balance);
    printf("  Deposit amount   : ");
    scanf("%f", &amount);

    if (amount <= 0) { printf("\n  >> Invalid amount!\n"); pressEnter(); return; }

    currentUser->balance += amount;
    recordTxn(currentUser, "Cash Deposit", amount, "CR");
    printf("\n  >> Deposit successful!\n");
    printf("  New Balance : %.2f TK\n", currentUser->balance);
    pressEnter();
}

/* ── E. WITHDRAW ── */
void withdrawMoney() {
    float amount;
    clearScreen();
    printHeader("WITHDRAW");
    printf("\n  Current Balance  : %.2f TK\n", currentUser->balance);
    printf("  Withdraw amount  : ");
    scanf("%f", &amount);

    if (amount <= 0)                   { printf("\n  >> Invalid amount!\n");       pressEnter(); return; }
    if (amount > currentUser->balance) { printf("\n  >> Insufficient balance!\n"); pressEnter(); return; }

    currentUser->balance -= amount;
    recordTxn(currentUser, "Cash Withdrawal", amount, "DR");
    printf("\n  >> Withdrawal successful!\n");
    printf("  New Balance : %.2f TK\n", currentUser->balance);
    pressEnter();
}

/* ── F. PAYMENT SYSTEM ── */
void paymentSystem() {
    char shopName[40], shopAcc[30];
    float amount;
    clearScreen();
    printHeader("PAYMENT - ONLINE SHOPPING");

    printf("\n  Current Balance         : %.2f TK\n", currentUser->balance);
    printf("\n  Enter shop name         : "); scanf("%39s", shopName);
    printf("  Enter shop account no.  : "); scanf("%29s", shopAcc);
    printf("  Enter payment amount    : "); scanf("%f",   &amount);

    if (amount <= 0)                   { printf("\n  >> Invalid amount!\n");       pressEnter(); return; }
    if (amount > currentUser->balance) { printf("\n  >> Insufficient balance!\n"); pressEnter(); return; }

    currentUser->balance -= amount;
    char desc[60];
    snprintf(desc, 59, "Payment->%.15s[%.10s]", shopName, shopAcc);
    recordTxn(currentUser, desc, amount, "DR");

    printf("\n  -------- PAYMENT RECEIPT --------\n");
    printf("  Shop          : %s\n",       shopName);
    printf("  Account No.   : %s\n",       shopAcc);
    printf("  Amount Paid   : %.2f TK\n",  amount);
    printf("  New Balance   : %.2f TK\n",  currentUser->balance);
    printf("  Status        : SUCCESS\n");
    pressEnter();
}

/* ── G. LOAN SYSTEM ── */
void loanSystem() {
    char  loanNames[3][3][30] = {
        {"House Loan",     "Car Loan",       "Education Loan"},
        {"Car Loan",       "Education Loan", ""},
        {"Education Loan", "",               ""}
    };
    float loanMax[3][3] = {
        {1000000.0f, 500000.0f, 200000.0f},
        { 300000.0f, 150000.0f,       0.0f},
        {  50000.0f,      0.0f,       0.0f}
    };
    int loanCount[3] = {3, 2, 1};

    float income;
    clearScreen();
    printHeader("LOAN SYSTEM");
    printf("\nEnter your monthly income: ");
    scanf("%f", &income);

    int tier;
    if      (income >= 50000) tier = 0;
    else if (income >= 20000) tier = 1;
    else                      tier = 2;

    printf("\n====== LOAN OPTIONS ======\n");
    for (int i = 0; i < loanCount[tier]; i++)
        printf("  %d. %-22s  Up to: %.0f TK\n", i+1, loanNames[tier][i], loanMax[tier][i]);
    printf("==========================\n");

    int ch;
    printf("\n  Select loan option (0 = cancel) : ");
    scanf("%d", &ch);
    if (ch < 1 || ch > loanCount[tier]) { printf("  >> Cancelled.\n"); pressEnter(); return; }

    float req;
    printf("  Enter loan amount requested     : ");
    scanf("%f", &req);

    if (req <= 0 || req > loanMax[tier][ch-1]) {
        printf("\n  >> Amount invalid or exceeds maximum (%.0f TK)!\n", loanMax[tier][ch-1]);
        pressEnter(); return;
    }

    currentUser->balance += req;
    char desc[60];
    snprintf(desc, 59, "Loan: %s", loanNames[tier][ch-1]);
    recordTxn(currentUser, desc, req, "CR");

    printf("\n  >> Loan of %.2f TK approved and credited!\n", req);
    printf("  New Balance : %.2f TK\n", currentUser->balance);
    pressEnter();
}

/* ── H. INTEREST SYSTEM ── */
void interestSystem() {
    clearScreen();
    printHeader("INTEREST SYSTEM");

    float bal  = currentUser->balance;
    float rate;

    if      (bal >= 100000) rate = 8.5f;
    else if (bal >=  50000) rate = 7.0f;
    else if (bal >=  20000) rate = 5.5f;
    else if (bal >=  10000) rate = 4.0f;
    else                    rate = 2.5f;

    printf("\n  Current Balance       : %.2f TK\n",   bal);
    printf("  Your Interest Rate    : %.1f%% p.a.\n", rate);
    printf("\n  -- Balance Range & Interest Rates --\n");
    printf("  100,000+ TK  -->  8.5%%\n");
    printf("   50,000+ TK  -->  7.0%%\n");
    printf("   20,000+ TK  -->  5.5%%\n");
    printf("   10,000+ TK  -->  4.0%%\n");
    printf("    < 10,000   -->  2.5%%\n");

    int years;
    printf("\n  Enter number of years : ");
    scanf("%d", &years);
    if (years <= 0) { printf("  >> Invalid!\n"); pressEnter(); return; }

    float future = compoundInterest(bal, rate, years);
    float earned = future - bal;

    printf("\n  -------- INTEREST SUMMARY --------\n");
    printf("  Principal        : %.2f TK\n", bal);
    printf("  Rate             : %.1f%% per year\n", rate);
    printf("  Years            : %d\n",      years);
    printf("  Interest Earned  : %.2f TK\n", earned);
    printf("  Total Amount     : %.2f TK\n", future);
    printf("  (Computed using recursive compound interest)\n");
    pressEnter();
}

/* ── MAIN MENU ── */
void userMenu() {
    int ch;
    do {
        clearScreen();
        printHeader("MAIN MENU");
        printf("\n  Logged in as : %s  |  Balance : %.2f TK\n\n",
               currentUser->username, currentUser->balance);

        printf("  [1]  Balance Check\n");
        printf("  [2]  Account Statement\n");
        printf("  [3]  Deposit Money\n");
        printf("  [4]  Withdraw Money\n");
        printf("  [5]  Online Payment\n");
        printf("  [6]  Loan System\n");
        printf("  [7]  Interest Calculator\n");
        printf("  [0]  Logout\n");
        printf("\n  Choice : ");
        scanf("%d", &ch);

        switch (ch) {
            case 1: checkBalance();   break;
            case 2: printStatement(); break;
            case 3: depositMoney();   break;
            case 4: withdrawMoney();  break;
            case 5: paymentSystem();  break;
            case 6: loanSystem();     break;
            case 7: interestSystem(); break;
            case 0:
                printf("\n  >> Logged out. Goodbye, %s!\n\n", currentUser->username);
                currentUser = NULL;
                break;
            default:
                printf("\n  >> Invalid choice!\n");
                pressEnter();
        }
    } while (ch != 0);
}

/* ── MAIN ── */
int main() {
    initMembers();

    int running = 1;
    while (running) {
        clearScreen();
        printLine('=', 62);
        printf("\n");
        printf("   BANK MANAGEMENT SYSTEM  |  CSE207  |  EWU\n\n");
        printLine('=', 62);
        printf("\n  [1] Login\n");
        printf("  [0] Exit\n");
        printf("\n  Choice : ");

        int opt;
        scanf("%d", &opt);

        if (opt == 1) {
            if (loginInterface())
                userMenu();
        } else if (opt == 0) {
            clearScreen();
            printf("\n  Thank you for using the Bank Management System!\n");
            printf("  CSE207 Data Structures | East West University\n\n");
            running = 0;
        } else {
            printf("\n  >> Invalid option!\n");
            pressEnter();
        }
    }

    Account *tmp = head;
    while (tmp) {
        Account *nxt = tmp->next;
        free(tmp);
        tmp = nxt;
    }

    return 0;
}
