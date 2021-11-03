// CMSC 341 - Spring 2021 - Project 4
// driver.cpp
// Simple driver program for use with HashTable class
#include "hash.h"
#include <random>

// Using a fixed seed value generates always the same sequence
// of pseudorandom numbers, e.g. reproducing scientific experiments
// here it helps us with testing since the same sequence repeats
std::mt19937 generator(10);// 10 is the fixed seed value

// We distribute the pseudorandom numbers uniformaly within
// the range [1000,99999]
std::uniform_int_distribution<> distribution(1000, 99999);

// The hash function used by HashTable class
unsigned int hashCode(const string str);

// the function to generate random serial numbers
unsigned int vaccineSerial() {return distribution(generator);}

class Tester{ // Tester class to implement test functions
   public:
   Tester(){}
   ~Tester(){}


    bool testInsertion(HashTable& table) {
       Person p1("jack", 1);
       Person p2("bob", 2);
       Person p3("mike", 3);
       Person p4("george", 4);
       Person p5("rob", 5);
       Person p6("carl", 6);

       table.insert(p1);
       table.insert(p2);
       table.insert(p3);
       table.insert(p4);
       table.insert(p5);
       table.insert(p6);
       table.insert(Person ("jo", 7));
       table.insert(Person ("sam", 8));

       if (table.m_table[table.m_hash("jack") % table.tableSize()].key() != "jack")
            return false;
       if (table.m_table[table.m_hash("jack") % table.tableSize()].serial() != 1)
           return false;
       if (table.m_table[table.m_hash("bob") % table.tableSize()].key() != "bob")
           return false;
       if (table.m_table[table.m_hash("bob") % table.tableSize()].serial() != 2)
           return false;
       if (table.m_table[table.m_hash("mike") % table.tableSize()].key() != "mike")
           return false;
       if (table.m_table[table.m_hash("mike") % table.tableSize()].serial() != 3)
           return false;
       if (table.m_table[table.m_hash("george") % table.tableSize()].key() != "george")
           return false;
       if (table.m_table[table.m_hash("george") % table.tableSize()].serial() != 4)
           return false;
       if (table.m_table[table.m_hash("rob") % table.tableSize()].key() != "rob")
           return false;
       if (table.m_table[table.m_hash("rob") % table.tableSize()].serial() != 5)
           return false;
       if (table.m_table[table.m_hash("carl") % table.tableSize()].key() != "carl")
           return false;
       if (table.m_table[table.m_hash("carl") % table.tableSize()].serial() != 6)
           return false;
       if (hashCode("carl") % table.tableSize() != table.m_hash(p6.key()) % table.tableSize())
           return false;

       return true;

    }

    bool testRemove(HashTable& table) {
        Person p1("jack", 1);
        Person p2("bob", 2);
        Person p3("mike", 3);
        Person p4("george", 4);
        Person p5("rob", 5);
        Person p6("carl", 6);

        table.insert(p1);
        table.insert(p2);
        table.insert(p3);
        table.insert(p4);
        table.insert(p5);
        table.insert(p6);
        table.insert(Person ("jo", 7));
        table.insert(Person ("sam", 8));

        for (int i = 0; i < table.tableSize(); i++) {
            if (table.m_table[i].key() != "" && !table.remove(table.m_table[i]))
                return false;
        }
        return true;
    }

    bool linearProbeInsertion(HashTable& table) {

       //make sure probe is linear
       if (table.m_currentProb != LINEAR)
           return false;

       int index = table.m_hash("Debbie"); 

       for (int i = 0; i < 50; i++) {
           table.insert(Person("Debbie", i));

           if (table.m_table[(index + i) % table.tableSize()].serial() != i)
            return false;
       }

       return true;
    }

    bool linearProbeRemoval(HashTable& table) {

       int index = table.m_hash("Debbie");

       //after 13 will rehash and be quadratic
       for (int i = 0; i < 13; i++) {
           if (table.m_table[(index + i) % table.tableSize()].serial() != i)
               return false;
           table.remove(Person ("Debbie", i));
       }

       if (table.m_currentProb != QUADRATIC)
           return false;

        return true;
    }

    bool quadProbeInsertion(HashTable &table) {
        int index = table.m_hash("Debbie"); 

        for (int i = 0; i < 51; i++) {
            table.insert(Person("Debbie", i));
        }
	
        //make sure table size changes correctly
	if (table.m_capacity != 211)
	  return false;

        if (table.m_currentProb != QUADRATIC)
            return false;

        for (unsigned int i = 51; i < 100; i++) {
            table.insert(Person("Debbie", i));

            if (table.m_table[((index % table.tableSize()) + (i * i))
                               % table.tableSize()].serial() != i)
                return false;
        }

        return true;
    }

    bool quadProbeRemoval(HashTable& table) {
        if (table.m_currentProb != QUADRATIC)
            return false;

        int index = table.m_hash("Debbie");

        //after 26 will rehash and be linear
        for (unsigned int i = 0; i < 26; i++) {

            //because of the rehashing, the first item in the array is 5 not 0 so add 5
            if (table.m_table[((index % table.tableSize()) + (i * i))
                              % table.tableSize() ].serial() != 5 + i)
                return false;
            table.remove(Person ("Debbie", i));
        }

        if (table.m_currentProb != LINEAR)
            return false;

        return true;
    }
};

int main(){
   // We use this temp array to store some data points for later use
   int tempSerials[50] = {0};
   int secondIndex = 0;
   // create a hash table with the size 101, and the hash function hashCode
   HashTable aTable(101,hashCode);
   int temp = 0;
   // we insert 50 Person objects with the same key, person's name is the key
   for (int i=0;i<50;i++){
      temp = vaccineSerial();// generates a random number
      if (i%3 == 0){//this saves 17 numbers from the generated numbers for later use
         tempSerials[secondIndex] = temp;
         cout << temp << " was saved for later use." << endl;
         secondIndex++;
      }
      // print the random numbers which are inserted
      cout << "Insertion # " << i << " => " << temp << endl;
      aTable.insert(Person("John", temp));
   }
   cout << "Message: dump after 50 insertions in a table with 101 buckets:" << endl;
   aTable.dump();
   
   // We insert a data point to change the load factor, this should initiate a rehash
   temp = vaccineSerial();
   cout << "Insertion # 51" << " => " << temp << endl;
   aTable.insert(Person("John", temp));
   cout << "Message: dump after 51st insertion, the rehash should have happened to a table with 211 buckets:" << endl;
   aTable.dump();
   
   // Now we remove some data points which were saved in tempSerials array
   // Deleting 13 data points changes the deleteRatio, it should initiate a rehash 
   for (int i = 0;i<14;i++)
      aTable.remove(Person("John", tempSerials[i]));
   cout << "Message: dump after 14 removes, a rehash should have happened to a table with 157 buckets," << endl;
   cout << "Message: after 13 removals the rehash happens, then the 14th removal will be tagged as DELETED in the new table." << endl;
   aTable.dump();
   cout << endl;

   HashTable bTable(3, hashCode);
   //try to insert the same serial number one after the other
   bTable.insert(Person("Deb", 1));
   if (!bTable.insert(Person("Deb", 1)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //try to insert the same serial number after insert another node, test probing
   bTable.insert(Person("Deb", 3));
   if (!bTable.insert(Person("Deb", 3)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //size was less then min prime, so making sure it was changed
   if (bTable.tableSize() == 3)
       cout << "Fail" << endl;
   else
       cout << "Pass" << endl;

   if (bTable.tableSize() == MINPRIME)
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //remove the objects in bTable
   if (bTable.remove(Person ("Deb", 3)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //try to remove object after already removed
   if (!bTable.remove(Person ("Deb", 3)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   if (bTable.remove(Person ("Deb", 1)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //try to remove on empty array
   if (!bTable.remove(Person ("Deb", 3)))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   Tester test;
   //testing insertion index
   if (test.testInsertion(bTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //testing removal
   HashTable cTable(3, hashCode);
   if (test.testRemove(cTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //testing linear probe
   HashTable dTable(3, hashCode);
   if (test.linearProbeInsertion(dTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //testing linear probe
   if (test.linearProbeRemoval(dTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //testing quad probe, insertion
   HashTable eTable(3, hashCode);
   if (test.quadProbeInsertion(eTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;

   //testing quad probe, removal
   if (test.quadProbeRemoval(eTable))
       cout << "Pass" << endl;
   else
       cout << "Fail" << endl;


    return 0;


}

// The implementation of the hash function
// you can copy this function to your mytest.cpp file
unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for ( int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}


