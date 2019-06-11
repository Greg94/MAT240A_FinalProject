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

public class CodeUpload : MonoBehaviour {

    [DllImport("AudioPluginCompiler")]
    private static extern bool Code_Upload([MarshalAs(UnmanagedType.LPStr)] string code);


    public AudioMixer mixer;
    //[DllImport("AudioPluginCompiler")]
    //private static extern int Code_Debug(byte[] buf);

    //[DllImport("AudioPluginCompiler")]
    //private static extern int Test_TCC();

    // Use this for initialization
    void Start () {
        string code = "Cycle(2000);";
        bool isUploaded = Code_Upload(code);
        Debug.Log(isUploaded);


        Debug.Log(mixer.SetFloat("Recompile", 0.5f));
        //byte[] buf = new byte[300];
        //Code_Debug(buf);
        //Debug.Log(System.Text.Encoding.ASCII.GetString(buf));
        //int val = Test_TCC();
        //Debug.Log(val);
    }

    // Update is called once per frame
    void Update () {

	}
}
