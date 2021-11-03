// CMSC 341 - Spring 2021 - Project 4
// hash.cpp
// The impleme ntation of the class HashTable
#include "hash.h"
#include "math.h"
HashTable::HashTable(unsigned size, hash_fn hash){
    //set capacity, making sure it's a prime number
    if (size < MINPRIME)
        size = MINPRIME;

    else if (size > MAXPRIME)
        size = MAXPRIME;

    if (!isPrime(size))
      size = findNextPrime(size);
    
    m_capacity = size;

    //pointer to hash function
    m_hash = hash;

    //pointer to array of people, dynamically allocated
    m_table = new Person[m_capacity];

    //initialize table
    for (int i = 0; i < m_capacity; i++) {
        m_table[i].setSerial(0);
	m_table[i].setKey("");
    }

    m_size = 0;
    m_numDeleted = 0;
    m_currentProb = LINEAR;

}

HashTable::~HashTable(){
   delete [] m_table;
   m_table = nullptr;

   m_size = 0;
   m_numDeleted = 0;
   m_currentProb = LINEAR;
}

bool HashTable::insert(Person person){
    int index = 0;
    
    //find insertion key
    index = m_hash(person.key()) % m_capacity;
    
    //person already in table, not probing
    if (m_table[index].key() == person.key() && m_table[index].serial() == person.serial())
        return false;

    //spot open, accounting for deleted buckets and 0 as a serial
    if (!m_table[index].serial() && m_table[index].key() != DELETEDKEY && m_table[index].key() == "") {
        m_table[index] = person;
        m_size++;

	//check if need to rehash table
        if (lambda() > 0.5)
            setProbingPolicy(m_currentProb);
        return true;
    }

    //COLLISION
    else {
      
        //check for next open spot, either linear or quadratic probing
        for (unsigned int i = 0; i < m_capacity; i++) {
            int lin = (m_hash(person.key()) + i) % m_capacity;
            int quadr = ((m_hash(person.key()) % m_capacity) + (i * i)) % m_capacity;
            index = (m_currentProb == LINEAR ? lin : quadr);

	    //person already in table
	    if (m_table[index].key() == person.key() && m_table[index].serial() == person.serial())
	      return false;

	    //spot open
            if (!m_table[index].serial() && m_table[index].key() != DELETEDKEY && m_table[index].key() == "") {
                m_table[index] = person;

                m_size++;

		//check if need to rehash
                if (lambda() > 0.5)
                    setProbingPolicy(m_currentProb);

                return true;
            }
        }
    }

    //wasn't inserted
    return false;
}

bool HashTable::remove(Person person){
    //find person
    int index;

    for (unsigned int i = 0; i < m_capacity; i++) {
        index = (m_currentProb == LINEAR ? (m_hash(person.key()) + i) % m_capacity :
                ((m_hash(person.key()) % m_capacity) + (i * i)) % m_capacity);

	//person found (key and serial matches), change person and key to deleted
	if (m_table[index].key() == person.key() && m_table[index].serial() == person.serial()) {
            m_table[index] = DELETED;
            m_table[index].setKey(DELETEDKEY);

            m_numDeleted++;

	    //see if need to rehash
            if (m_numDeleted > (.25 * m_size))
                setProbingPolicy(m_currentProb);

            return true;
        }
    }

    //person not deleted
    return false;
}

void HashTable::setProbingPolicy(probing prob){
    m_size -= m_numDeleted;
    m_numDeleted = 0;

    //put people objects into an array
    Person* personArray = new Person[m_size];
    array(personArray);

    delete [] m_table;
    m_table = nullptr;

    //get new capacity size
    int newCapacity = 4 * m_size;

    if (newCapacity < MINPRIME)
      newCapacity = MINPRIME;
    if (newCapacity > MAXPRIME)
      newCapacity = MAXPRIME;
    
    if (!isPrime(newCapacity))
        newCapacity = findNextPrime(newCapacity);
    m_capacity = newCapacity;

    //declare and initialize new table
    m_table = new Person[m_capacity];
    for (int i = 0; i < m_capacity; i++) {
        m_table[i].setSerial(0);
	m_table[i].setKey("");
    }

    //switch probing
    if (prob == LINEAR) {
        m_currentProb = QUADRATIC;
    }
    else if (prob == QUADRATIC){
        m_currentProb = LINEAR;
    }

    //rehash table
    unsigned int size = m_size;
    m_size = 0;
    rehash(personArray, size);

    delete [] personArray;
}

void HashTable::array(Person* personArray) {
    int counter = 0;
    
    //go through the whole old array
    for (int i = 0; i < m_capacity; i++) {

        //put all values in from old array into new array
        if ((m_table[i].serial() || m_table[i].key() != "") && m_table[i].key() != DELETEDKEY) {
            personArray[counter].setSerial(m_table[i].serial());
            personArray[counter].setKey(m_table[i].key());
            counter ++;
        }
    }
}

void HashTable::rehash(Person* personArray, unsigned int size) {
    //insert temp array values into new array
    for (unsigned int i = 0; i < size; i++) {
        insert(personArray[i]);
    }
}

float HashTable::lambda() const {
     return (float(m_size)/float(m_capacity));
}

float HashTable::deletedRatio() const {
    return (float(m_numDeleted)/float(m_capacity));
}

void HashTable::dump() const {
  for (int i = 0; i < m_capacity; i++) {
    cout << "[" << i << "] : " << m_table[i] << endl;
  }
}

bool HashTable::isPrime(int number){
    // If number is prime this function returns true
    // otherwise it returns false
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int HashTable::findNextPrime(int current){
    //we won't go beyond MAXPRIME
    //the smallest prime would be next after MINPRIME
    if (current < MINPRIME) current = MINPRIME;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}




