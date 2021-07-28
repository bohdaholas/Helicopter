# define MAX_VAL 255
# define BUFFER_SIZE 15
//typedef int buffer[BUFFER_SIZE];

// buffer engine;

int engine[BUFFER_SIZE];

void setup(){
    Serial.begin(9600);
    pinMode(A0, OUTPUT);

    for(int i = 0; i < BUFFER_SIZE; i++){
        engine[i] = 0;
    }
}

float computeBufferAverage(int * buff, int buffSize){
    int buffSum = 0;
    for(int i = 0; i < buffSize; ++i) buffSum += buff[i];

    return (float) buffSum / buffSize;
}

int * intToBinSeq(int num){
    // This functino transforms int number to sequence of binary 0 and 1 according to MAX_VAL (binary vector)
    // e.g. : MAX_VAL = 8, num = 5. intToBinSeq(num) = 10110101

    static int binSeq[MAX_VAL];

    const float frequency = (float) (MAX_VAL - 1) / num;    // frequency of zeros in binary sequence
    float local_frequency = frequency;
    int i = 0;
    for(; i < MAX_VAL; ++i){
        if(i == (int) local_frequency){
            binSeq[i] = 1;
            local_frequency += frequency;
        }
        else{
            binSeq[i] = 0;
        }
    }

    return binSeq;
}

bool isClose(double a, double b, double EPSILON){
    double lhs = a > b ? a-b : b-a;
    return lhs < EPSILON;
}

int counter = 0;

void loop(){
    // delay is required -- otherwise the loop passes too fast and low/high changes to pin does not
    // manage to fit in such a short tiem gap
    delay(10);
    counter = counter%BUFFER_SIZE;
    int * binVector;
    binVector = intToBinSeq(3);
    
    float memoryBufferAvr = computeBufferAverage(engine, BUFFER_SIZE);
    float requiredBufferAvr = computeBufferAverage(binVector, MAX_VAL);
    // printf("%f vs %f\n", memoryBufferAvr, requiredBufferAvr);

    // Serial.print("requiredBufferAvr: ");
    // Serial.print(requiredBufferAvr);
    // Serial.print(" (");
    // Serial.print(requiredBufferAvr >= 0.00);
    // Serial.print(") ->");

    const double EPSILON = 0.01;

    if(isClose(requiredBufferAvr, 0, EPSILON)) {
        analogWrite(A0, 0);
        engine[counter] = 0;
        Serial.println(0);
    }
    else if(memoryBufferAvr > requiredBufferAvr){
        analogWrite(A0, 0);
        engine[counter] = 0;
        Serial.println(0);
    }
    else{
        analogWrite(A0, 255);
        engine[counter] = 1;
        Serial.println(255);
    }
    counter++;
}
