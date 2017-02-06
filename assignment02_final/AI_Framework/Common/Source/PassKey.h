#ifndef PASS_KEY_H
#define PASS_KEY_H

template <typename Type>
struct PassKey {
	//Friend(s)
	friend Type;
private:
	//Constructor(s)
	PassKey() {}
};

template <typename Type, typename Type2>
struct PassKey2 {
	//Friend(s)
	friend Type;
	friend Type2;
private:
	//Constructor(s)
	PassKey2() {}
};

template <typename Type, typename Type2, typename Type3>
struct PassKey3 {
	//Friend(s)
	friend Type;
	friend Type2;
	friend Type3;
private:
	//Constructor(s)
	PassKey3() {}
};

template <typename Type, typename Type2, typename Type3, typename Type4>
struct PassKey4 {
	//Friend(s)
	friend Type;
	friend Type2;
	friend Type3;
	friend Type4;
private:
	//Constructor(s)
	PassKey4() {}
};

template <typename Type, typename Type2, typename Type3, typename Type4, typename Type5>
struct PassKey5 {
	//Friend(s)
	friend Type;
	friend Type2;
	friend Type3;
	friend Type4;
	friend Type5;
private:
	//Constructor(s)
	PassKey5() {}
};

#endif