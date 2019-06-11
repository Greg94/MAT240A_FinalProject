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

using UnityEditor;
using UnityEngine;

public class CodeEditor : EditorWindow
{
    string code = "Hello World";
    Vector2 scroll;
    GUIStyle style;
    Texture2D texture_;
    //EditorWindow window;

    // Add menu item named "My Window" to the Window menu
    [MenuItem("Window/CodeEditor")]
    public static void ShowWindow()
    {
        //Show existing window instance. If one doesn't exist, make one.
        EditorWindow.GetWindow(typeof(CodeEditor));
    }



//    Hello World


//cycle(440, 1, 0) %sine oscillator: 440 Hz, Amp 1, Phase Offset 0

//for (int i = 0; i< [insertDopeC++VariableName]; i++) {
//	%do stuff
//}


void OnGUI()
    {
        GUILayout.Label("Code Editor", EditorStyles.boldLabel);

        if (texture_ == null)
        {
            texture_ = blackTexture();
        }

        style = new GUIStyle();
        style.richText = true;

        style.normal.background = texture_;
        style.normal.textColor = Color.white;

        GUI.skin.settings.selectionColor = new Color(0,1,1,.6f);
        GUI.skin.settings.cursorColor = Color.white;

        //GUI.color = Color.black;

        scroll = EditorGUILayout.BeginScrollView(scroll);
        code = EditorGUILayout.TextArea(code, style);
        EditorGUILayout.EndScrollView();

        //code = ParseString(code);

    }


    Texture2D blackTexture()
    {
        Texture2D tex = new Texture2D(10, 10);

        // Reset all pixels color to transparent
        Color32 resetColor = new Color32(0, 0, 0, 255);
        Color32[] resetColorArray = tex.GetPixels32();

        for (int i = 0; i < resetColorArray.Length; i++)
        {
            resetColorArray[i] = resetColor;
        }

        Debug.Log("making black background");

        tex.SetPixels32(resetColorArray);
        tex.Apply();
        return tex;
    }

    //string ParseString(string code)
    //{
        //return string.Concat("<b>", code, "</b >");
    //}

}