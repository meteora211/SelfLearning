#include "vector_adt.h"
#include <string>

int main()
{
	int intarray[10] = {1,3,4,6,2,5,0,9,7,8};
	std::string stringarray[5] = {"Student", "Zhang", "Li", "Wei", "Wang"};

	Vector<int> default_v(5,5,1);
	Vector<int> copy_vi(intarray, 0, 10);
	Vector<std::string> copy_vs(stringarray, 1, 5);

	default_v.Print();
	copy_vi.Print();
	copy_vs.Print();

	std::cout << "capacity of default_v: " << default_v.capacity() << std::endl;
	//This will cause an error since expand is protected member function which is not allowed to be accessed by an object.
	//default_v.expand();    
	std::cout << "default_v.get(0)=" << default_v.get(0) << std::endl;
	std::cout << "intarray[2]=" << intarray[2] << std::endl;
	std::cout << "capacity after exand: " << default_v.capacity() << std::endl;
	std::cout << "unsort string array: " << std::endl;
	copy_vs.unsort(0,5);
	copy_vs.Print();
	std::cout << "find 9 in copy_vi " << copy_vi.find(9) << std::endl;
	std::cout << "insert 10 in copy_vi " << std::endl;
	copy_vi.insert(1,10);
	copy_vi.Print();
	std::cout << "capacity after insert: " << copy_vi.capacity() << std::endl;

	std::cout << "copy_vi.remove(2,5)" << std::endl;
	copy_vi.remove(2,5);
	copy_vi.Print();
	std::cout << "capacity after remove: " << copy_vi.capacity() << std::endl;
	//	std::cout << "randnumber: " << rand() % 10 << std::endl;
	//std::cout << "deduplicate default_v " << std::endl;
	//default_v.deduplicate();
	//default_v.Print();
	//std::cout << "capacity of default_v after deduplicate" << default_v.capacity() << std::endl;
	
	// test traverse
	Increase<int> increaseStruct;
	default_v.traverse(increaseStruct);
	default_v.Print();
	Vector<int> test(5,5,2);

	increase(test);
//	Increase<int>();
	test.Print();
	std::cout << "uniquify test " << std::endl;
	test.uniquify();
	test.Print();

	std::cout << "binsearch 6 in copy_vi " << copy_vi.search(6,0,7) << std::endl;
	std::cout << "binsearch 9 in copy_vi " << copy_vi.search(9,0,7) << std::endl;
	return 0;
}
