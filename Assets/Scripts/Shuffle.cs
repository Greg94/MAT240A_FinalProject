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

public class Shuffle : MonoBehaviour {

    public bool shuffle = false;
    private bool prevState = true;
    private CodeBox box;

	// Use this for initialization
	void Start () {
        box = GetComponent<CodeBox>();
	}
	
	// Update is called once per frame
	void Update () {
        if (shuffle == prevState)
        {
            ShuffleCodeBox();
            prevState = !prevState;
        }
	}

    void ShuffleCodeBox()
    {
        string[] codeArray = box.code.Split(';');
        Debug.Log(codeArray.Length);
        int nLines = codeArray.Length - 1;
        //string[] shuffledArray = new string[nLines];

        for (int i = 0; i <nLines; i++)
        {
            Debug.Log(codeArray[i]);
        }

        
        //codeArray[nLines - 1] = codeArray[nLines - 1].Substring(0, codeArray[nLines-1].Length - 1);

        //Knuth shuffle algorithm
        for (int i = 0; i < nLines; i++)
        {
            string temp = codeArray[i];
            int randIdx = Random.Range(i, nLines);
            codeArray[i] = codeArray[randIdx];
            codeArray[randIdx] = temp;
        }
        box.code = string.Join(";",codeArray); //rejoin code array
        
    }
}
