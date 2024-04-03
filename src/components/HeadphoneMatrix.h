#include <Adafruit_NeoMatrix.h>
#include <definitions/Icons.h>

class HeadphoneMatrix {
    private:
        Adafruit_NeoMatrix *matrix;
        
    public:
        HeadphoneMatrix();
        ~HeadphoneMatrix();
        void setup();
        void display(uint16_t color);
};