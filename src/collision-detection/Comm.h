

typedef struct duck duck;
struct duck { };

int test();
duck* new_duck(int feet);
void delete_duck(duck* d);
void duck_quack(duck* d, float volume);


class Duck : public duck {
public:
	Duck(int feet);
	~Duck();

	void quack(float volume);
};