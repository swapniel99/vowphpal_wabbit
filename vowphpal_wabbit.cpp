#include <phpcpp.h>
#include <vwdll.h>
#include <cmath>
#include <fstream>


// The class with static values loaded at beginning of each Apache instance
class VowPHPal_Wabbit : public Php::Base
{
    private:
        static void* _modelPointer;	// Pointer to LR model in memory
        static int _counter;		// Count of number of predictions

        // Takes care of calling VW returning the score for a given example
        static Php::Value predict1(const char* exstring)
        {
            Php::Value score=0f;
            if(_modelPointer==NULL)
                return score;
            void* example = VW_ReadExampleA(_modelPointer, exstring);
            score = VW_Predict(_modelPointer, example);
            VW_FinishExample(_modelPointer, example);
            _counter++;
            return score;
        }

    public:

	    // Default Constructor
	    VowPHPal_Wabbit() {}
        
	    // Destructor
	    virtual ~VowPHPal_Wabbit() {}

        //Initialize static model using "--quiet -t -i /path/to/model"
        static void initializeStaticModel(Php::Parameters &params)
        {
           if(_modelPointer != NULL)
           {
               _counter = 0;
               VW_Finish(_modelPointer);
           }
           _modelPointer = VW_InitializeA((const char*)(params[0].rawValue()));
        }

        //End model
        static void finishStaticModel()
        {
            _counter = 0;
            VW_Finish(_modelPointer);
        }

        //Check for model
        static PHP::Value isModelPresent()
        {
            if(_modelPointer==NULL)
                return 0;
            else
                return 1;
        }

        // Get prediction of a single example.
        static Php::Value getPrediction(Php::Parameters &params)
        {
            Php::Value score = predict1((const char*)(params[0].rawValue()));
            return score;
        }

        // Get predictions for an array of examples.
        static Php::Value getnPredictions(Php::Parameters &params)
        {
            std::vector<Php::Value> res;
            std::vector<Php::Value> exampleArray = params[0];
            for (Php::Value &exampleStr : exampleArray)
            {
                Php::Value score = predict1(exampleStr.rawValue());
                res.push_back(score);
            }
            return (Php::Value)res;
        }
        
	    // Get number of predictions done since model was initialized
	    static Php::Value getCounter()
        {
            return _counter;
        }
};


// Static Member Initialisation
void* VowPHPal_Wabbit::_modelPointer = NULL;
int VowPHPal_Wabbit::_counter = 0;

/**
 *  tell the compiler that the get_module is a pure C function
 */

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
        static Php::Extension vwextension("vowphpal_wabbit", "1.1");
        
        // @todo    add your own functions, classes, namespaces to the extension
        Php::Class<VowPHPal_Wabbit> vowphpal_wabbit("VowPHPal_Wabbit");
        
    	vowphpal_wabbit.method("initializeStaticModel", &VowPHPal_Wabbit::initializeStaticModel, {
                Php::ByRef("exampleString", Php::Type::String)
                });


    	vowphpal_wabbit.method("finishStaticModel", &VowPHPal_Wabbit::finishStaticModel, {});

    	vowphpal_wabbit.method("isModelPresent", &VowPHPal_Wabbit::isModelPresent, {});

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
