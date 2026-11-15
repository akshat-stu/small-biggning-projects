#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ACCOUNT_FILE "accounts.dat"
#define TRANSACTION_FILE "transactions.dat"

typedef struct
{
    unsigned long long accNo;
    char name[50];
    char pin[5];
    double balance;
    int active;
} Account;

typedef struct
{
    unsigned long long fromAcc;
    unsigned long long toAcc;
    double amount;
    char type[20];
    time_t timestamp;
} Transaction;

// Function declarations
void createAccount(void);
void userLogin(void);
void adminLogin(void);
void userMenu(Account acc);
void adminMenu(void);
void depositMoney(unsigned long long accNo);
void withdrawMoney(unsigned long long accNo);
void transferMoney(unsigned long long accNo);
void checkBalance(unsigned long long accNo);
void transactionHistory(unsigned long long accNo);
void viewAllAccounts(void);
void recordTransaction(unsigned long long from,
                       unsigned long long to,
                       double amount,
                       const char *type);

int findAccount(unsigned long long accNo,
                Account *acc, long *position);

int saveAccount(Account *acc, long position);

// Find account and its file position
int findAccount(unsigned long long accNo,
                Account *acc, long *position)
{
    FILE *fp = fopen(ACCOUNT_FILE, "rb");

    if (fp == NULL)
        return 0;

    Account temp;
    long pos;

    while (1)
    {
        pos = ftell(fp);

        if (fread(&temp, sizeof(Account), 1, fp) != 1)
            break;

        if (temp.accNo == accNo && temp.active == 1)
        {
            *acc = temp;
            *position = pos;
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// Update existing account
int saveAccount(Account *acc, long position)
{
    FILE *fp = fopen(ACCOUNT_FILE, "rb+");

    if (fp == NULL)
        return 0;

    if (fseek(fp, position, SEEK_SET) != 0)
    {
        fclose(fp);
        return 0;
    }

    int result = fwrite(acc, sizeof(Account), 1, fp);
    fclose(fp);

    return result == 1;
}

// Generate next account number
unsigned long long generateAccountNumber(void)
{
    FILE *fp = fopen(ACCOUNT_FILE, "rb");

    Account temp;
    unsigned long long max = 10000;

    if (fp != NULL)
    {
        while (fread(&temp, sizeof(Account), 1, fp) == 1)
        {
            if (temp.accNo > max)
                max = temp.accNo;
        }

        fclose(fp);
    }

    return max + 1;
}

// Create new account
void createAccount(void)
{
    Account acc = {0};

    acc.accNo = generateAccountNumber();
    acc.active = 1;

    printf("\n--- CREATE NEW ACCOUNT ---\n");

    printf("Enter customer name: ");
    getchar();
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = '\0';

    if (strlen(acc.name) == 0)
    {
        printf("Name cannot be empty!\n");
        return;
    }

    printf("Create 4-digit PIN: ");
    scanf("%4s", acc.pin);

    if (strlen(acc.pin) != 4)
    {
        printf("PIN must contain exactly 4 digits!\n");
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        if (!isdigit((unsigned char)acc.pin[i]))
        {
            printf("PIN must contain digits only!\n");
            return;
        }
    }

    printf("Enter initial deposit: ");
    if (scanf("%lf", &acc.balance) != 1 ||
        acc.balance < 0)
    {
        printf("Invalid amount!\n");
        return;
    }

    FILE *fp = fopen(ACCOUNT_FILE, "ab");

    if (fp == NULL)
    {
        printf("File opening error!\n");
        return;
    }

    int result = fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    if (result != 1)
    {
        printf("Account creation failed!\n");
        return;
    }

    printf("\nAccount created successfully!\n");
    printf("Account Number: %llu\n", acc.accNo);
    printf("Account Holder: %s\n", acc.name);
    printf("Balance: %.2lf\n", acc.balance);
}

// User login
void userLogin(void)
{
    unsigned long long number;
    char pin[20];

    Account acc;
    long position;

    printf("\nEnter account number: ");
    scanf("%llu", &number);

    printf("Enter PIN: ");
    scanf("%19s", pin);

    if (findAccount(number, &acc, &position) &&
        strcmp(acc.pin, pin) == 0)
    {
        printf("\nLogin successful!\n");
        userMenu(acc);
    }
    else
    {
        printf("\nInvalid account number or PIN!\n");
    }
}

// Record transaction in file
void recordTransaction(unsigned long long from,
                       unsigned long long to,
                       double amount,
                       const char *type)
{
    FILE *fp = fopen(TRANSACTION_FILE, "ab");

    if (fp == NULL)
    {
        printf("Transaction record error!\n");
        return;
    }

    Transaction t = {0};

    t.fromAcc = from;
    t.toAcc = to;
    t.amount = amount;
    t.timestamp = time(NULL);

    snprintf(t.type, sizeof(t.type), "%s", type);

    fwrite(&t, sizeof(Transaction), 1, fp);

    fclose(fp);
}

// Deposit money
void depositMoney(unsigned long long accNo)
{
    Account acc;
    long position;
    double amount;

    if (!findAccount(accNo, &acc, &position))
        return;

    printf("Enter deposit amount: ");

    if (scanf("%lf", &amount) != 1 ||
        amount <= 0 ||
        amount > 1000000000.0 ||
        acc.balance > 1000000000.0 - amount)
    {
        printf("Invalid deposit amount!\n");
        return;
    }

    acc.balance += amount;

    if (!saveAccount(&acc, position))
    {
        printf("Account update failed!\n");
        return;
    }

    recordTransaction(0, accNo, amount, "DEPOSIT");

    printf("Deposit successful!\n");
    printf("New balance: %.2lf\n", acc.balance);
}

// Withdraw money
void withdrawMoney(unsigned long long accNo)
{
    Account acc;
    long position;
    double amount;

    if (!findAccount(accNo, &acc, &position))
        return;

    printf("Enter withdrawal amount: ");

    if (scanf("%lf", &amount) != 1 ||
        amount <= 0 ||
        amount > 1000000000.0)
    {
        printf("Invalid amount!\n");
        return;
    }

    if (amount > acc.balance)
    {
        printf("Insufficient balance!\n");
        return;
    }

    acc.balance -= amount;

    if (!saveAccount(&acc, position))
    {
        printf("Account update failed!\n");
        return;
    }

    recordTransaction(accNo, 0, amount, "WITHDRAW");

    printf("Withdrawal successful!\n");
    printf("Remaining balance: %.2lf\n", acc.balance);
}

// Transfer money
void transferMoney(unsigned long long accNo)
{
    Account sender, receiver;
    long senderPos, receiverPos;
    unsigned long long target;
    double amount;

    if (!findAccount(accNo, &sender, &senderPos))
        return;

    printf("Enter receiver account number: ");
    scanf("%llu", &target);

    if (target == accNo ||
        !findAccount(target, &receiver, &receiverPos))
    {
        printf("Invalid receiver account!\n");
        return;
    }

    printf("Enter transfer amount: ");

    if (scanf("%lf", &amount) != 1 ||
        amount <= 0 ||
        amount > 1000000000.0)
    {
        printf("Invalid amount!\n");
        return;
    }

    if (amount > sender.balance)
    {
        printf("Insufficient balance!\n");
        return;
    }

    if (receiver.balance > 1000000000.0 - amount)
    {
        printf("Receiver balance limit exceeded!\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    if (!saveAccount(&sender, senderPos))
    {
        printf("Sender update failed!\n");
        return;
    }

    if (!saveAccount(&receiver, receiverPos))
    {
        printf("Receiver update failed. Manual recovery may be needed!\n");
        return;
    }

    recordTransaction(accNo, target, amount, "TRANSFER");

    printf("Money transferred successfully!\n");
    printf("Your remaining balance: %.2lf\n",
           sender.balance);
}

// Check balance
void checkBalance(unsigned long long accNo)
{
    Account acc;
    long position;

    if (findAccount(accNo, &acc, &position))
    {
        printf("\nAccount Holder: %s\n", acc.name);
        printf("Account Number: %llu\n", acc.accNo);
        printf("Available Balance: %.2lf\n", acc.balance);
    }
}

// Transaction history
void transactionHistory(unsigned long long accNo)
{
    FILE *fp = fopen(TRANSACTION_FILE, "rb");

    if (fp == NULL)
    {
        printf("No transaction history found!\n");
        return;
    }

    Transaction t;
    int found = 0;

    printf("\n--- TRANSACTION HISTORY ---\n");

    while (fread(&t, sizeof(Transaction), 1, fp) == 1)
    {
        if (t.fromAcc == accNo || t.toAcc == accNo)
        {
            char *date = ctime(&t.timestamp);

            printf("\nType: %s\n", t.type);
            printf("Amount: %.2lf\n", t.amount);

            if (date != NULL)
                printf("Date: %s", date);

            printf("----------------------\n");
            found = 1;
        }
    }

    if (!found)
        printf("No transactions available!\n");

    fclose(fp);
}

// User dashboard
void userMenu(Account acc)
{
    int choice;

    do
    {
        printf("\n========== USER DASHBOARD ==========\n");
        printf("1. Check Balance\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Transfer Money\n");
        printf("5. Transaction History\n");
        printf("6. Logout\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            return;
        }

        switch (choice)
        {
            case 1:
                checkBalance(acc.accNo);
                break;

            case 2:
                depositMoney(acc.accNo);
                break;

            case 3:
                withdrawMoney(acc.accNo);
                break;

            case 4:
                transferMoney(acc.accNo);
                break;

            case 5:
                transactionHistory(acc.accNo);
                break;

            case 6:
                printf("Logged out successfully!\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 6);
}

// Admin panel
void adminLogin(void)
{
    char username[30], password[30];

    printf("\nEnter admin username: ");
    scanf("%29s", username);

    printf("Enter admin password: ");
    scanf("%29s", password);

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "1234") == 0)
    {
        printf("\nAdmin login successful!\n");
        adminMenu();
    }
    else
    {
        printf("Invalid admin credentials!\n");
    }
}

// View all accounts and total deposits
void viewAllAccounts(void)
{
    FILE *fp = fopen(ACCOUNT_FILE, "rb");

    if (fp == NULL)
    {
        printf("No accounts available!\n");
        return;
    }

    Account acc;
    double total = 0;
    int count = 0;

    printf("\n--- ALL BANK ACCOUNTS ---\n");

    while (fread(&acc, sizeof(Account), 1, fp) == 1)
    {
        if (acc.active == 1)
        {
            printf("\nAccount No: %llu\n", acc.accNo);
            printf("Name: %s\n", acc.name);
            printf("Balance: %.2lf\n", acc.balance);

            total += acc.balance;
            count++;
        }
    }

    printf("\nTotal Accounts: %d\n", count);
    printf("Total Deposits: %.2lf\n", total);

    fclose(fp);
}

// Admin dashboard
void adminMenu(void)
{
    int choice;

    do
    {
        printf("\n========== ADMIN PANEL ==========\n");
        printf("1. View All Accounts\n");
        printf("2. Logout\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
            return;

        switch (choice)
        {
            case 1:
                viewAllAccounts();
                break;

            case 2:
                printf("Admin logged out!\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 2);
}

// Main function
int main(void)
{
    int choice;

    do
    {
        printf("\n====================================\n");
        printf("       BANK MANAGEMENT SYSTEM       \n");
        printf("====================================\n");

        printf("1. Create New Account\n");
        printf("2. User Login\n");
        printf("3. Admin Login\n");
        printf("4. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                createAccount();
                break;

            case 2:
                userLogin();
                break;

            case 3:
                adminLogin();
                break;

            case 4:
                printf("Thank you for using our bank!\n");
                break;

            default:
                printf("Please enter a valid choice!\n");
        }

    } while (choice != 4);

    return 0;
}