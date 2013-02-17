#ifndef UNITTEST_H
#define UNITTEST_H
///////////////////////////////////////////////////////////////
// UnitTest.h - inline helper functions for unit testing     //
// ver 1.0                                                   //
// --------------------------------------------------------- //
// copyright © Jim Fawcett, 2012                             //
// All rights granted provided that this notice is retained  //
// --------------------------------------------------------- //
// Jim Fawcett, Summer Projects, 2012                        //
///////////////////////////////////////////////////////////////

inline bool passed(const std::string& name)
{
  std::cout << "\n  passed: " << name;
  return true;
}

inline bool failed(const std::string& name)
{
  std::cout << "\n  failed: " << name;
  return false;
}

inline bool checkResult(bool predicate, const std::string& msg)
{
  if(predicate)
    return passed(msg);
  else
    return failed(msg);
}

template <typename T>
class ITest
{
public:
  typedef bool (T::* pTest) ();

  virtual ~ITest() {}

  bool doTest(pTest atest) { 
  //bool doTest(T& t, pTest atest) { 
    try
    {
      return (dynamic_cast<T*>(this)->*atest)();
      //return (t.*atest)();
    }
    catch (std::exception& ex)
    {
      std::cout << "\n  failed: " << ex.what() << " - thrown exception";
      return false;
    }
  }
  virtual bool test()=0;
};
/*
 * This is what a test class should look like:
 *
 class TestSomeClass : public ITest<TestSomeClass>
 {
 public:
   bool aTest()
   {
     bool t1, t2;
     // create instances of SomeClass and test them
     // call static methods of SomeClass and test them
     t1 = true;  // result of testing not shown here
     t2 = false; // result of testing not shown here
     return checkResult(t1 && t2, "TestSomeClassATest");
   }
   bool anotherTest()
   {
     bool t1;
     t1 = true;  // result of testing not shown here
     return checkResult(t1, "TestSomeClassAnotherTest");
   }
   bool test()  // overriding inherited test method
   {
     bool t1, t2;
     t1 = doTest(&TestSomeClass::aTest);
     t2 = doTest(&TestSomeClass::anotherTest);
     return t1 && t2;
   }
 private:
   // save here data needed in more than one test 
   // as member data
 };
 *
 * You may wisht to cut and past this sample in your test driver.
 */
#endif
