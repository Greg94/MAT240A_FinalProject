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


using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using UnityEngine.Audio;


public class CodeBox : MonoBehaviour {

    public AudioMixer mixer;
    public string floatParamName;

    [TextArea (maxLines:10, minLines:10)]
    public string code; //user written code
    private string currentCode; //for comparing changes made

    [DllImport("AudioPluginCompiler")]
    private static extern bool Code_Upload([MarshalAs(UnmanagedType.LPStr)] string code);


    // Use this for initialization
    void Start()
    {
        currentCode = code;
        bool isUploaded = Code_Upload(code);
        mixer.SetFloat(floatParamName, Random.Range(0f, 1f));
        Debug.Log(isUploaded);
    }

    // Update is called once per frame
    void Update () {
        bool upload = CompareCode(currentCode, code);
        if (upload)
        {
            code = code.Replace("\r", string.Empty).Replace("\n", string.Empty).Replace(" ",string.Empty);
            bool isUploaded = Code_Upload(code);
            mixer.SetFloat(floatParamName, Random.Range(0f, 1f));
            currentCode = code;
            Debug.Log(currentCode);
        }
    }

    bool CompareCode(string c1, string c2)
    {
        if (c1.Equals(c2) == false)
        {
            return true;
        }
        else return false;
    }
}
