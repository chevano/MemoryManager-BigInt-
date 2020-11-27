#include <iostream>
#include <string>
#include <cstring> // Needed Depending on your compiler(strcpy)

using namespace std;

#define POOLSIZE 99

class MemMgr {
    private:
        struct FS {
            FS* next;
        };

        void increasePoolSize();
        void destroy();
        FS* head;

    public:
        MemMgr();
        virtual ~MemMgr();
        virtual void* allocate(size_t);
        virtual void free(void*);
};

MemMgr mm;

class BigInt {
   private:
    int *num;
    int size;

   public:
    BigInt();
    BigInt(string);
    BigInt(int);
    BigInt(const BigInt &);
    ~BigInt();

    // Set to inline for quick dispatch
    inline void* operator new(size_t); 
    inline void* operator new[](size_t); 
    inline void operator delete(void *);
    inline void operator delete[](void *);

    void normalizeSize(BigInt&, BigInt&);
    int isLarger(const BigInt&, const BigInt&);
    void computeSum(BigInt&, BigInt&, BigInt&);
    void computeDiff(BigInt&, BigInt&, BigInt&, int&);
    void computeMul(BigInt&, BigInt&, BigInt&);
    void setIsSize(const int&, const int&, int&, bool&, int&);
    void setSign(const int&, const int&, const int&, const int&, BigInt&);
    void removeLeadingZeros(BigInt&);
    void padNumber(BigInt&, BigInt&);
    BigInt reverse(const BigInt&);
    BigInt sub(const BigInt& lhs, const BigInt& rhs);

    BigInt operator+(const BigInt&);
    BigInt operator-(const BigInt&);
    BigInt operator*(const BigInt&);
    int operator/(const BigInt&);
    friend ostream& operator<<(ostream& os, const BigInt &);

    BigInt& operator=(const BigInt&);
};

int main()
{
    // BigInt b1("12345678901234");
    // BigInt b2("12345678901234");
    BigInt b1("8888");
    BigInt b2("9999");

    BigInt b3 = b1 + b2;
    BigInt b4 = b1 - b2;
    BigInt b5 = b2 - b1;
    BigInt b6 = b1 * b2;
    int b7 = b2 / b1;

    cout << b1 << " + " << b2 << " = " << b3 << endl;
    cout << b1 << " - " << b2 << " = " << b4 << endl;
    cout << b2 << " - " << b1 << " = " << b5 << endl;
    cout << b1 << " * " << b2 << " = " << b6 << endl;
    cout << b2 << " / " << b1 << " = " << b7 << endl;
    
    return 0; 
}

MemMgr::MemMgr() {
    head = 0;
    increasePoolSize();
}

MemMgr::~MemMgr() {
    destroy();
}

inline void* MemMgr::allocate(size_t size) {
    // Checks whether the free store has been
    // created, if not then it will create it
    if(head == 0)
        increasePoolSize();

    FS* newHead = head;
    head = newHead->next;
    
    return newHead;
}

inline void MemMgr::free(void* ptr) {
    FS* newHead = static_cast <FS*> (ptr);
    newHead->next = head;
    head = newHead;
}

void MemMgr::increasePoolSize() {
    // Since the free-store pointer FS* is used
    // as a BigInt Object, the size requested for
    // individual free-store pointers must be as follows
    size_t size = max(sizeof(BigInt),sizeof(FS*));
    FS* newHead = reinterpret_cast <FS*> (new char[size]);
    head = newHead;

    for(int i = 0; i < POOLSIZE; i++) {
        newHead->next = reinterpret_cast <FS*> (new char[size]);
        newHead = newHead->next;
    }

    newHead->next = 0;
}

/******************************
*   Removes all the pointers in
*   the free store.
*******************************/
void MemMgr::destroy() {
    FS* nextPtr = head;

    while(nextPtr) {
        head = head->next;
        delete [] nextPtr;
        nextPtr = head;
    }
}

void* BigInt::operator new (size_t size) {
  return mm.allocate(size);
}

void* BigInt::operator new[] (size_t size) {
  return mm.allocate(size);
}

void BigInt::operator delete (void* pointer) {
  mm.free(pointer);
}
void BigInt::operator delete[] (void* pointer) {
  mm.free(pointer);
}

/********************************
*       Default Constructor
*********************************/
BigInt::BigInt() {
    size = 1000;
    num = new int[size];
}

/********************************
*       Generic Constructor
*********************************/
BigInt::BigInt(int size) {
    this->num = new int[size];
    this->size = size;

    for(int i = 0; i < size; i++)
        num[i] = 0;
}

/********************************
*       Generic Constructor
*********************************/
BigInt::BigInt(string str) {
    this->size = str.length();
    this->num = new int[size];

    for(int i = 0; i < size; i++)
        num[i] = str[i] - '0';
}

/********************************
*       Copy Constructor
*********************************/
BigInt::BigInt(const BigInt & oldObj) {
    num = new int[oldObj.size];
    size = oldObj.size;

    for(int i = 0; i < oldObj.size; i++)
        num[i] = oldObj.num[i];
}

/********************************
*        Destructor
*********************************/
BigInt::~BigInt() {
    delete [] num;
}

/*********************************************
*   Copies the element from the smaller object
*   to the newly created larger object
**********************************************/
void BigInt::normalizeSize(BigInt& largerObj, BigInt& smallerObj) {
    int sizeDiff = largerObj.size - smallerObj.size;

    for(int i = sizeDiff; i < largerObj.size; i++)
        largerObj.num[i] = smallerObj.num[i-sizeDiff];
}

/**************************************************
*       returns -1 if lhs < rhs
*       returns 0 if lhs = rhs
*       returns 1 if lhs > rhs
**************************************************/
int BigInt::isLarger(const BigInt& lhs, const BigInt& rhs) {
    
    int length = lhs.size;
    
    for(int i = 0; i < length; i++) {
        if(lhs.num[i] > rhs.num[i])
            return 1;
            
        else if(lhs.num[i] < rhs.num[i])
            return -1;
    }
    
    return 0;
}

/**************************************************
*               Reverses a BigInt
**************************************************/
BigInt BigInt::reverse(const BigInt &bigInt) {
    BigInt temp;
    temp.num = new int[bigInt.size];
    temp.size = bigInt.size;
    
    int index = 0;

    for(int i = bigInt.size - 1; i >= 0; i--) {
        temp.num[index++] = bigInt.num[i];
    }

    return temp;
}

/**************************************************
*       Removes all the leading zeros
*       in a BigInt Object
**************************************************/
void BigInt::removeLeadingZeros(BigInt &bigInt) {
    int index = -1;
    
    // Finds the first occurence of a non-zero element
    for(int i = 0; i < bigInt.size; i++) {
        if(bigInt.num[i] != 0) {
            index = i;
            break;
        }
    }
    // Check whether the array is filled with all zeros
    if(index == -1) {
        delete [] bigInt.num;
        bigInt.num = new int[1];
        bigInt.size = 1;
        bigInt.num[0] = 0;
    }

    // Creates a new array with a reduce size where all the leading zeros are removed
    else {
        int reduceSize = bigInt.size - index;
        int *temp = new int[reduceSize];

        for(int i = 0; i < reduceSize; i++)
            temp[i] = bigInt.num[index + i];

        delete [] bigInt.num;
        bigInt.num = new int[reduceSize];
        bigInt.size = reduceSize;

        for(int i = 0; i < reduceSize; i++)
            bigInt.num[i] = temp[i];

        delete [] temp;
    }
}

/**************************************************
*       Computes the sum of two BigInt
**************************************************/
void BigInt::computeSum(BigInt& bigInt, BigInt& lhsTemp, BigInt& rhsTemp) {
    for(int i = 0; i < bigInt.size; i++) {
        bigInt.num[i] += lhsTemp.num[i] + rhsTemp.num[i];

        if(bigInt.num[i] > 9) {
            bigInt.num[i] = bigInt.num[i] % 10;
            bigInt.num[i+1] += 1;
        }
    }  
}

/**************************************************
*       Computes the product of two BigInt
**************************************************/
void BigInt::computeMul(BigInt& bigInt, BigInt& lhsTemp, BigInt& rhsTemp) {
    int carry, i, j;
    for(i = 0; i < lhsTemp.size; i++) {
        carry = 0;
        for(j = 0; j < rhsTemp.size; j++) {
            int x = lhsTemp.num[i] * rhsTemp.num[j] + bigInt.num[i+j];
            carry = x / 10;
            bigInt.num[i+j] = x % 10;
            bigInt.num[i+j+1] += carry;
        }
    }
   
}

/**************************************************
*       Computes the difference of two BigInt
**************************************************/
void BigInt::computeDiff(BigInt& bigInt, BigInt& lhsTemp, BigInt& rhsTemp, int& largerObject) {
    for(int i = 0; i < bigInt.size; i++) {
        if(largerObject == -1) {
            if(rhsTemp.num[i] >= lhsTemp.num[i])
                bigInt.num[i] = rhsTemp.num[i] - lhsTemp.num[i];
            else {
                rhsTemp.num[i+1] -= 1;
                rhsTemp.num[i] += 10;
                bigInt.num[i] = rhsTemp.num[i] - lhsTemp.num[i];
            }
        }

        else {
            if(lhsTemp.num[i] >= rhsTemp.num[i])
                bigInt.num[i] = lhsTemp.num[i] - rhsTemp.num[i];
                
            else {
                lhsTemp.num[i+1] -= 1;
                lhsTemp.num[i] += 10;
                bigInt.num[i] = lhsTemp.num[i] - rhsTemp.num[i];
            }
        }
    }
}

/**************************************************
*       Sets the variables sign, isSize,
*       largerSize depending on the size
*       of the lhs object and the rhs object
**************************************************/
void BigInt::setIsSize(const int& lhsSize, const int& rhsSize, int& largerSize, bool& isSize, int& sign) {
    if(lhsSize >= rhsSize) {
        sign = 1;
        isSize = true;
        largerSize = lhsSize;
    }

    else {
        sign = -1;
        isSize = false;
        largerSize = rhsSize;
    }
}

void BigInt::setSign(const int& lhsSize, const int& rhsSize, const int& sign, const int& largerObject, BigInt& result) {
    if(lhsSize < rhsSize)
        result.num[0] *= sign;
    else {
        if(largerObject != 0)
            result.num[0] *= largerObject;
    }
}

/**************************************************
*       Checks whether the two object are of the
*       same size. If they aren't then the
*       smaller object gets pad with zeros
*       at the front so that both objects are
*       of the same size
**************************************************/
void BigInt::padNumber(BigInt& lhs, BigInt& rhs) {

    int largerSize, largerObject, sign;

    // Set to true if the object on the left hand side
    // size is greater than or equal to the object on
    // the right hand side
    bool isSize;

    setIsSize(lhs.size, rhs.size, largerSize, isSize, sign);

    BigInt temp(largerSize);
    BigInt bigInt(largerSize);

    int sizeDiff = abs(lhs.size - rhs.size);
    int index = 0;

    for(int i = sizeDiff; i < largerSize; i++) {
        if(isSize)
            temp.num[i] = rhs.num[index++];
        else
            temp.num[i] = lhs.num[index++];
    }

    if(isSize) {
        delete [] rhs.num;
        rhs.num = NULL;
        rhs.size = temp.size;
        rhs.num = new int[temp.size];

        for(int i = 0; i < rhs.size; i++) {
            rhs.num[i] = temp.num[i];
        }
    }
        
    else {
        delete [] lhs.num;
        lhs.num = NULL;
        lhs.size = temp.size;
        lhs.num = new int[temp.size];

        for(int i = 0; i < lhs.size; i++) {
            lhs.num[i] = temp.num[i];
        }
    }
}

/**************************************************
*       Overload the extraction operator 
*       for printing a BigInt
**************************************************/
ostream& operator<<(ostream& os, const BigInt& bigInt) {
    for(int i = 0; i < bigInt.size; i++) {
        os << bigInt.num[i];
    }

    return os;
}

/**************************************************
*       Overload the = operator 
*       for storing a BigInt into another BigInt
**************************************************/
BigInt& BigInt::operator=(const BigInt& obj) {
    // Checks whether the object on the left hand side
    // is different from the object on the right hand side
    // of the = operator
    if(this != &obj) {
        delete [] num;
        size = obj.size;
        num = new int[size];

        for(int i = 0; i < obj.size; i++)
            num[i] = obj.num[i];
    }

    return *this;
}

/**************************************************
*       Overload the + operator 
*       to perform addition 
**************************************************/
BigInt BigInt::operator+(const BigInt& rhs) { 
    int largerSize, sign, index = 0;
    bool isSize; 

    BigInt lhsCopy = *this; // Object on the left hand side of the + operator
    BigInt rhsCopy = rhs;   // Object on the right hand side of the + operator

    setIsSize(lhsCopy.size, rhsCopy.size, largerSize, isSize, sign);

    BigInt lhsCopyy(largerSize+1);
    BigInt rhsCopyy(largerSize+1);

    // int lhsDiff = lhsCopyy.size - lhsCopy.size;
    
    // for(int i = lhsDiff; i < lhsCopyy.size; i++)
    //     lhsCopyy.num[i] = lhsCopy.num[i-lhsDiff];

    // int rhsDiff = rhsCopyy.size - rhsCopy.size;

    // for(int i = rhsDiff; i < rhsCopyy.size; i++)
    //     rhsCopyy.num[i] = rhsCopy.num[i-rhsDiff];

    normalizeSize(lhsCopyy, lhsCopy);
    normalizeSize(rhsCopyy, rhsCopy);

    BigInt lhsTemp, rhsTemp;

    lhsTemp = reverse(lhsCopyy);
    rhsTemp = reverse(rhsCopyy);

    BigInt bigInt(largerSize+1);
    computeSum(bigInt,lhsTemp,rhsTemp);
    BigInt result = reverse(bigInt);
    removeLeadingZeros(result);

    return result;
}

/**************************************************
*       Overload the - operator 
*       to perform subtraction 
**************************************************/
BigInt BigInt::operator-(const BigInt& rhs) { 

    BigInt lhsCopy = *this;   // Object on the left hand side of the - operator
    BigInt rhsCopy = rhs;   // Object on the right hand side of the - operator

    return sub(lhsCopy,rhsCopy);
}

/**************************************************
*       Overload the * operator 
*       to perform multiplication 
**************************************************/
BigInt BigInt::operator*(const BigInt& rhs) {
    int largerSize, sign, index = 0;

    // Set to true if the object on the left hand side
    // size is greater than or equal to the object on
    // the right hand side
    bool isSize; 

    BigInt lhsCopy = *this;   // Object on the left hand side of the * operator
    BigInt rhsCopy = rhs;   // Object on the right hand side of the * operator

    setIsSize(lhsCopy.size, rhsCopy.size, largerSize, isSize, sign);

    BigInt lhsTemp = reverse(lhsCopy);
    BigInt rhsTemp = reverse(rhsCopy);


    BigInt bigInt(largerSize * 2);
    computeMul(bigInt, lhsTemp, rhsTemp);
    BigInt result = reverse(bigInt);
    removeLeadingZeros(result);

    return result;
}

int BigInt::operator/(const BigInt& rhs) {
    int counter = 0;

    BigInt lhsCopy = *this;
    BigInt rhsCopy = rhs;

    padNumber(lhsCopy, rhsCopy);

    // Checks how many time we can 
    // subtract the rhs value from the lhs value
    while(isLarger(lhsCopy, rhsCopy) >= 0) {
        lhsCopy = sub(lhsCopy, rhsCopy);
        counter++;
        padNumber(lhsCopy, rhsCopy);
    }

    return counter;
}

/**************************************************
*       Given two objects lhs, and rhs,
*       where lhs is the object on the
*       left hand size, and rhs is the onject on
*       the right hand size. The following operation
*       is performed lhs - rhs
**************************************************/
BigInt BigInt::sub(const BigInt& lhs, const BigInt& rhs) { 

    int largerSize, largerObject, sign;

    // Set to true if the object on the left hand side
    // size is greater than or equal to the object on
    // the right hand side
    bool isSize;

    BigInt lhsCopy = lhs;   // Object on the left hand side of the - operator
    BigInt rhsCopy = rhs;   // Object on the right hand side of the - operator

    setIsSize(lhsCopy.size, rhsCopy.size, largerSize, isSize, sign);

    BigInt temp(largerSize);
    BigInt bigInt(largerSize);

    int sizeDiff = abs(lhsCopy.size - rhsCopy.size);
    int index = 0;

    for(int i = sizeDiff; i < largerSize; i++) {
        if(isSize)
            temp.num[i] = rhsCopy.num[index++];
        else
            temp.num[i] = lhsCopy.num[index++];
    }

    BigInt lhsTemp, rhsTemp;
    if(isSize) {
        largerObject = isLarger(lhsCopy,temp);
        lhsTemp = reverse(lhsCopy);
        rhsTemp = reverse(temp);
    }
        
    else {
        largerObject = isLarger(temp,rhsCopy);
        lhsTemp = reverse(temp);
        rhsTemp = reverse(rhsCopy);
    }

    computeDiff(bigInt, lhsTemp, rhsTemp, largerObject);
    BigInt result = reverse(bigInt);
    removeLeadingZeros(result);
    setSign(lhsCopy.size, rhsCopy.size, sign, largerObject, result);

    return result;
}
