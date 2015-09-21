#include <phpcpp.h>
#include <vwdll.h>
#include <cmath>
#include <fstream>

/**
 *  tell the compiler that the get_module is a pure C function
 */

float* initializeSigmoid()
{
    float* pars = (float*)malloc(2*sizeof(float));
    std::ifstream sigfile("/home/ec2-user/sigpars.txt");
    if(sigfile.good())
        sigfile >> pars[0] >> pars[1];
    sigfile.close();
    return pars;
}

float sigmoid(float x, float a, float b)
{
    return (1/(1+exp(b-a*x)));
}

class VowPHPal_Wabbit : public Php::Base
{
    private:
        static void* _modelPointer;
	static float* _sigPars;
        static int _counter;

        static Php::Value getctr(const char* exstring)
        {
            void* example = VW_ReadExampleA(_modelPointer, exstring);
            float score = VW_Predict(_modelPointer, example);
            VW_FinishExample(_modelPointer, example);
            Php::Value ctr = sigmoid(score, _sigPars[0], _sigPars[1]);
            return ctr;
        }

    public:
        VowPHPal_Wabbit() {}
        virtual ~VowPHPal_Wabbit() {}
        static Php::Value getPrediction(Php::Parameters &params)
        {
            Php::Value score = getctr((const char*)(params[0].rawValue()));
            _counter++;
            return score;
        }
        static Php::Value getnPredictions(Php::Parameters &params)
        {
            std::vector<Php::Value> res;
            std::vector<Php::Value> exampleArray = params[0];
            for (Php::Value &exampleStr : exampleArray)
            {
                Php::Value score = getctr(exampleStr.rawValue());
                res.push_back(score);
                _counter++;
            }
            return (Php::Value)res;
        }
        static Php::Value getCounter()
        {
            return _counter;
        }
};

void* VowPHPal_Wabbit::_modelPointer = VW_InitializeA("--quiet -t -i /home/ec2-user/model.vw");
float* VowPHPal_Wabbit::_sigPars = initializeSigmoid();
int VowPHPal_Wabbit::_counter = 0;

extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension vwextension("vowphpal_wabbit", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        Php::Class<VowPHPal_Wabbit> vowphpal_wabbit("VowPHPal_Wabbit");
        vowphpal_wabbit.method("getPrediction", &VowPHPal_Wabbit::getPrediction, {
                Php::ByRef("exampleString", Php::Type::String)
                });
        vowphpal_wabbit.method("getnPredictions", &VowPHPal_Wabbit::getnPredictions, {
                Php::ByRef("exampleStringArray", Php::Type::Array)
                });
        vowphpal_wabbit.method("getCounter", &VowPHPal_Wabbit::getCounter);

        vwextension.add(std::move(vowphpal_wabbit));
        
        // return the extension
        return vwextension;
    }
}
