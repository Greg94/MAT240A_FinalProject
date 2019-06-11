/*
This program is a part of U-C++ a Unity Native Audio Plugin for audio synthesis.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see<https://www.gnu.org/licenses/>.
*/


#include "AudioPluginUtil.h"
#include<string>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include<list>
#include<iterator>
//#include "libtcc.h"


//#if _MSC_VER // this is defined when compiling with Visual Studio
//#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
//#else
//#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
//#endif


namespace TestCode {

	std::string  debug_mess = "Hello";
	std::string delimeter = ";";
	std::string functionDelim1 = "(";
	std::string functionDelim2 = ")";
	std::string paramDelim = ",";
	

	struct CodeString 
	{
		char* strCode;
		int allocated;
	};

	struct ParsedCode 
	{
		int nOperations = 0;
		int operations[10] = {0};
		int primitives[10] = { 0 };
		float params[20] = {0};
		//std::list<float> params; Note: Lists do not work in Native Audio Pluginss
	};

	ParsedCode ParseCode(std::string code) {
		ParsedCode parsed;

		int nIter = 0;
		int nParams = 0;

		//for each line
		while (code.find(delimeter) != std::string::npos) {

			std::size_t endIdx = code.find(delimeter);
			std::string line = code.substr(0, endIdx); //grab a line

			std::size_t paramStartIdx = line.find(functionDelim1);
			std::size_t paramEndIdx = line.find(functionDelim2);
			std::string token = line.substr(0, paramStartIdx);
			char op = line[0];




			int nParam = 0;
			int opType = 0;
			int primType = 0;

			/*
			if (op == '*')
			{
				opType = 1;
			}
			else if (op == '+')
			{
				opType = 2;
			}
			else 
			{
				code = code.substr(endIdx + 1, -1); //grab rest of code
				continue;
			}
			*/

			if ((strcmp(token.c_str(), "Cycle") == 0) || (strcmp(token.c_str(), "+Cycle") == 0))
			{
				primType = 1;
				opType = 1;
				nParam = 2;
			}
			else if (strcmp(token.c_str(), "*Cycle") == 0)
			{
				primType = 1;
				opType = 2;
				nParam = 2;
			}
			else if (strcmp(token.c_str(), "%Cycle") == 0)
			{
				primType = 1;
				opType = 3;
				nParam = 2;
			}
			else if ((strcmp(token.c_str(), "Noise") == 0) || (strcmp(token.c_str(), "+Noise") == 0))
			{
				primType = 2;
				opType = 1;
				nParam = 1;
			}
			else if (strcmp(token.c_str(), "*Noise") == 0)
			{
				primType = 2;
				opType = 2;
				nParam = 1;
			}
			else
			{
				code = code.substr(endIdx + 1, -1); //grab rest of code
				continue;
			}

			if ((paramStartIdx + 1 != paramEndIdx) && (paramStartIdx != std::string::npos) && (paramEndIdx != std::string::npos))
			{
				std::string params = line.substr(paramStartIdx + 1, paramEndIdx);

				//if only 1 param for valid line then.... - noise generator
				if (nParam == 1)
				{
					float p = std::stof(params);
					if (p > 0.0f) {
						parsed.params[nParams] = p;
					}
					else {
						parsed.params[nParams] = 0.0f;
					}
					parsed.nOperations += 1;
					parsed.primitives[nIter] = primType;
					parsed.operations[nIter] = opType;
					nIter += 1;
					nParams += 1;
				}

				// if 2 params - sine wave oscillator
				else
				{
					std::size_t subParamEndIdx = params.find(paramDelim);
					std::size_t subParamEndIdx2 = line.find(paramDelim);
					if (subParamEndIdx != std::string::npos && subParamEndIdx != 0 && subParamEndIdx2+1 != paramEndIdx)
					{
						std::string param1 = params.substr(0, subParamEndIdx);
						std::string param2 = params.substr(subParamEndIdx+1, -1);

						float p1 = std::stof(param1);
						if (p1 > 0.0f) {
							parsed.params[nParams] = p1;
						}
						else {
							parsed.params[nParams] = 0.0f;
						}

						float p2 = std::stof(param2);
						if (p2 > 0.0f) {
							parsed.params[nParams + 1] = p2;
						}
						else {
							parsed.params[nParams + 1] = 0.0f;
						}
						parsed.nOperations += 1;
						parsed.primitives[nIter] = primType;
						parsed.operations[nIter] = opType;
						nIter += 1;
						nParams += 2;

					}
					else
					{
						code = code.substr(endIdx + 1, -1); //grab rest of code
						continue;
					}
				}
			}
			code = code.substr(endIdx + 1, -1); //grab rest of code

		}

		return parsed;
	}

	//I dont know why this works
	inline CodeString& GetCodeString() 
	{
		static bool initialized = false;
		static CodeString code[1];
		//static CodeString* code
		if (!initialized) 
		{
			memset(code, 0, sizeof(code));
			initialized = true;
		}
		return code[0];
	};

	enum Param
	{
		P_GAIN,
		P_RECOMPILE,
		P_NUM
	};

	struct EffectData
	{
		float p[P_NUM]; // Parameters
		float time[10] = {0};
		float dt[10] = { 0 };
		ParsedCode parser;
		//int nOperations;
		//int operations[10] = {0,0,0,0,0,0,0,0,0,0};
		//int operations[10];

		//TCCState* tcc;
	};

	int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
	{
		int numparams = P_NUM;
		definition.paramdefs = new UnityAudioParameterDefinition[numparams];
		RegisterParameter(definition, "Gain", "",
			0.0f, 2.0f, 1.0f,
			1.0f, 1.0f,
			P_GAIN);

		RegisterParameter(definition, "Recompile", "",
			0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_RECOMPILE);
		return numparams;
	}

	UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
	{
		EffectData* effectdata = new EffectData;
		memset(effectdata, 0, sizeof(EffectData));
		effectdata->p[P_GAIN] = 1.0f;   // Set default parameter value(s)
		effectdata->p[P_RECOMPILE] = 0.0f;   // Set default parameter value(s)
		//effectdata->time = 0.0f;

		state->effectdata = effectdata; // Add our effectdata to the state so we can reach it in other callbacks
		InitParametersFromDefinitions(InternalRegisterEffectDefinition, effectdata->p);
		srand(time(nullptr)); // Seed the random number generator
		return UNITY_AUDIODSP_OK;
	}

	UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
	{
		EffectData *data = state->GetEffectData<EffectData>();
		delete data;
		return UNITY_AUDIODSP_OK;
	}

	UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
	{
		EffectData *data = state->GetEffectData<EffectData>();
		if (index >= P_NUM)
			return UNITY_AUDIODSP_ERR_UNSUPPORTED;
		data->p[index] = value;

		
		if (index == 1) {
			ParsedCode parsed;
			CodeString* s = &GetCodeString();
			if (s->allocated == 0)
			{
				const char* tCode("Init;");
				s->strCode = new char[strlen(tCode)];
				s->allocated = 1;
				memcpy(s->strCode, tCode, strlen(tCode) * sizeof(char));
			}
			//delete s->strCode;
			//}
			//else {

			data->parser = ParseCode(std::string(s->strCode));
		}
		

	 //do string parsing and generate fill the necessary effect data structures

		return UNITY_AUDIODSP_OK;
	}

	UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr)
	{
		EffectData *data = state->GetEffectData<EffectData>();
		if (index >= P_NUM)
			return UNITY_AUDIODSP_ERR_UNSUPPORTED;
		if (value != NULL)
			*value = data->p[index];
		if (valuestr != NULL)
			valuestr[0] = 0;
		return UNITY_AUDIODSP_OK;
	}

	int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples)
	{
		return UNITY_AUDIODSP_OK;
	}

	// ProcessCallback() gets called as long as the plugin is loaded
	// This includes when the editor is not in play mode!
	UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(
		UnityAudioEffectState* state,
		float* inbuffer, float* outbuffer,
		unsigned int length,
		int inchannels, int outchannels)
	{

		// Grab the EffectData struct we created in CreateCallback()
		EffectData *data = state->GetEffectData<EffectData>();

		// A gain multiplier to silence the plugin when not in play mode or muted
		float wetTarget = ((state->flags & UnityAudioEffectStateFlags_IsPlaying) && !(state->flags & (UnityAudioEffectStateFlags_IsMuted | UnityAudioEffectStateFlags_IsPaused))) ? 1.0f : 0.0f;

		int nParam = 0;
		for (int i =0; i< data->parser.nOperations; i++) 
		{
			if (data->parser.primitives[i] == 1) {
				data->dt[i] = data->parser.params[nParam] * 2 * 3.14159265358979323846f / state->samplerate;
				nParam = nParam + 2;
			}
			else {
				nParam = nParam + 1;
			}
		}

		for (unsigned int n = 0; n < length; n++)
		{
			float s = 0;
			
			nParam = 0;
			for (int i = 0; i < data->parser.nOperations; i++) 
			{
				//sine wave
				if (data->parser.primitives[i] == 1) 
				{
					if (data->parser.operations[i] == 1) {
						s = s + (sinf(data->time[i])  * data->parser.params[nParam + 1]);
					}
					else if (data->parser.operations[i] == 2) {
						s = s * (sinf(data->time[i])  * data->parser.params[nParam + 1]);
					}
					else {
						s = sinf(data->time[i] + s) * data->parser.params[nParam+1];
					}
					data->time[i] += data->dt[i];
					nParam += 2;
				}
				//noise
				else if (data -> parser.primitives[i] == 2) 
				{
					int rInt = rand() % 20000;
					rInt -= 10000;
					if (data->parser.operations[i] == 1) {
						s = s + (rInt / 10000.0f * data->parser.params[nParam]);
					}
					else {
						s = s * (rInt / 10000.0f * data->parser.params[nParam]);
					}
					nParam += 1;
				}
			}

			//fill output buffer
			for (int i = 0; i < outchannels; i++) 
			{
				outbuffer[n * outchannels + i] = s * wetTarget;
			}

		}

		return UNITY_AUDIODSP_OK;
	}
}

//Pass a string from Unity to Native Audio Plugin
extern "C" UNITY_AUDIODSP_EXPORT_API bool Code_Upload(char* code) 
{

	if (strlen(code) <= 0) 
	{
		return false;
	}

	TestCode::CodeString& s = TestCode::GetCodeString();
	if (s.allocated) 
	{
		delete s.strCode;
	}
	
	s.strCode = new char[strlen(code)];
	s.allocated = 1;
	memcpy(s.strCode, code, strlen(code) * sizeof(char)); //copy to plugin instance

	return true;
}


//return a string to Unity
extern "C" UNITY_AUDIODSP_EXPORT_API void Code_Debug(char* buf ) {
	std::string out = TestCode::debug_mess;
	strcpy_s(buf, sizeof(buf) ,out.c_str());
}




/*
//void tcc_error_handler(void* tcc, const char* msg);

struct TCC {
	using FunctionPointer = char(*)(int);
	FunctionPointer process = nullptr;
	TCCState* instance = nullptr;
	std::string error;

	void destroy() {
		if (instance) {
			tcc_delete(instance);
		}
	}

	bool compile() {
		destroy();
		instance = tcc_new();
		//assert(instance != nullptr);

		// set up the compiler
		//tcc_set_options(instance, "-nostdinc -nostdlib -Wall -Werror");
		//tcc_set_error_func(instance, this, tcc_error_handler);
		tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);
		//

		//test
		tcc_compile_string(instance, "char foo(int t) { return t; }");
		tcc_relocate(instance, TCC_RELOCATE_AUTO);
		tcc_delete(instance);

		return true;

		/*
		if (tcc_compile_string(instance, source.c_str()) == -1) {
			//
			// error string is set by the TCC handler
			return false;
		}

		if (tcc_relocate(instance, TCC_RELOCATE_AUTO) < 0) {
			error = "failed to relocate code";
			return false;
		}

		FunctionPointer foo = (FunctionPointer)(tcc_get_symbol(instance, "foo"));
		if (foo == nullptr) {
			error = "could not find the symbol 'foo'";
			return false;
		}

		// maybe we should go a step further and try a few calls to see if it
		// crashes

		error = "";
		process = foo;


		return true;

	}

	float operator()(int t) {
		if (process == nullptr) return 0;
		char c = process(t);
		return c / 128.0f;
	}
};
*/


//void tcc_error_handler(void* tcc, const char* msg) {
//	((TCC*)tcc)->error = msg;
	// TODO:
	// - remove file name prefix which is "<string>"
	// - correct line number which is off by about 20
//}

//I dont know why this works
//return active TCC instance
/*
inline TCC& GetTCC(int idx)
{
	static bool initialized = false;
	static TCC tcc[1];
	//static CodeString* code
	if (!initialized)
	{
		memset(tcc, 0, sizeof(tcc));
		initialized = true;
	}
	return tcc[idx];
};
*/

//Pass a string from Unity to Native Audio Plugin
/*
extern "C" {
	int EXPORT_API Test_TCC()
	{
		//test start code
		const char* starterCode = R"(
	char foo(int t) {
	  static int v = 0;
	  return (v=(v>>1)+(v>>4)+t*(((t>>16)|(t>>6))&(69&(t>>9))));
	  // return (t*((t>>12|t>>8)&63&t>>4));
	  // return (t*((t>>5|t>>8)>>(t>>16)));
	  // return (t*((t>>9|t>>13)&25&t>>6));
	  // return (t*(t>>11&t>>8&123*t>>3));
	  // return (t*(t>>8*(t>>15|t>>8)&(20|(t>>19)*5>>t|t>>3)));
	  // return (((-t&4095)*(255&t*(t&t>>13))>>12)+(127&t*(234&t>>8&t>>3)>>(3&t>>14)));
	  // return (t*(t>>((t>>9|t>>8))&63&t>>4));
	  // return ((t>>6|t|t>>(t>>16))*10+((t>>11)&7));
	  // return ((t|(t>>9|t>>7))*t&(t>>11|t>>9));
	  // return (t*5&(t>>7)|t*3&(t*4>>10));
	  // return ((t>>7|t|t>>6)*10+4*(t&t>>13|t>>6));
	  // return (((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t)));
	  // return (((t*(t>>8|t>>9)&46&t>>8))^(t&t>>13|t>>6));
	}
	)";




		//int out = TestCode::testTCC();
		//TestCode::TCC& tcc = TestCode::GetTCC(0);
		//	int testTCC() {

		//Mutex instanceMutex;
		//MutexScopeLock mutexScope(TCCState* instance = tcc_new());
		TCCState* instance = tcc_new();
		tcc_set_options(instance, "-nostdinc -nostdlib -Wall -Werror");
		//tcc_set_options(instance, "/MD testString.cpp -I libtcc libtcc.lib");
		tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);
		if (tcc_compile_string(instance, "char foo(int t) { return t; }") == -1) {
			return false;
		}

		//tcc_run(instance )
		//tcc_set_lib_path(instance, )

		//if (tcc_compile_string(instance, source.c_str()) == -1) {
			//
			// error string is set by the TCC handler
		//	return false;
		//}

		int s = tcc_relocate(instance, TCC_RELOCATE_AUTO);
		//if (s == -1) {
		//	printf("+ tcc_relocate() error\n");
		//	exit(2);
		//}


		//void *p1 = malloc(tcc_relocate(instance,NULL));

		//if (tcc_relocate(instance, p1) < 0) {
		//	return 1;
		//}
		//char(*foo)(int) = (char(*)(int))tcc_get_symbol(instance, "foo");
		int val = 1;
		//for (int t = 0; t < 10; t++) {
		//	int val = foo(t);
		//}
		//	cout << (int)foo(t) << endl;
		//tcc_delete(instance);


		tcc_delete(instance);
		return val;
		//return 0;
		//}

		//if (tcc.instance)
		//{
		//}

		//check this line - not copying string over to buffer first. Bad practice??

		//bool isCompiled = tcc.compile();
		//if (isCompiled)
		//{
		//	return true;
		//}

		//if compile failed
		//return out;
	}
}
*/


//if empty parenthesis
	//if (empty(params)) {
	//	parsed.params[nParams] = 0.0f;
	//	nParams += 1;
	//	break;
	//}

	/*
	if (paramStartIdx + 1 != paramEndIdx) {
		std::string params = line.substr(paramStartIdx + 1, paramEndIdx);
		while (true) {

			//multiple params
			if (params.find(paramDelim) != std::string::npos) {
				std::size_t subParamEndIdx = code.find(paramDelim);

				if ((subParamEndIdx == 0) || (strlen(params.c_str()) <= subParamEndIdx + 1))
				{
					break;
				}
				std::string param = params.substr(0, subParamEndIdx);
				float p = std::stof(param);
				if (p > 0.0f) {
					parsed.params[nParams] = p;
				}
				else {
					parsed.params[nParams] = 0.0f;
				}
				nParams += 1;

				params = params.substr(subParamEndIdx + 1, -1);
			}

			//single param
			else
			{
				float p = std::stof(params);
				if (p > 0.0f) {
					parsed.params[nParams] = p;
				}
				else {
					parsed.params[nParams] = 0.0f;
				}
				nParams += 1;
				break;
			}
		}
	}

	code = code.substr(endIdx + 1, -1); //grab rest of code
 }
 */

 //std::string line = code.substr(0, code.find(delimeter)); //get up to first semicolon
 //if (strcmp(line.c_str(), "Cycle") == 0) {
 //	parsed.operations[0] = 1;
 //	parsed.nOperations = 1;
 //}
 //parsed.test = 1;
