using System.Collections.Generic;
using GoogleARCore;
using UnityEngine;

#if UNITY_EDITOR
    using Input = GoogleARCore.InstantPreviewInput;
#endif

public class ObjectManipulationController : MonoBehaviour {
    // True if the app is in the process of quitting due to an ARCore error
    private bool m_IsQuitting = false;

    public void Update() {
        _UpdateApplicationLifecycle();
    }

    // Check and update the application lifecycle.
    private void _UpdateApplicationLifecycle()
    {
        if (Input.GetKey(KeyCode.Escape)) { 
            Application.Quit();
        }

        // Only allow the screen to sleep when not tracking
        if (Session.Status != SessionStatus.Tracking) {
            const int lostTrackingSleepTimeout = 15;
            Screen.sleepTimeout = lostTrackingSleepTimeout;
        } else {
            Screen.sleepTimeout = SleepTimeout.NeverSleep;
        }

        if (m_IsQuitting) {
            return;
        }

        // Quit if ARCore was unable to connect
        if (Session.Status == SessionStatus.ErrorPermissionNotGranted) {
            _ShowAndroidToastMessage("Camera permission is needed to run this application.");
            m_IsQuitting = true;
            Invoke("_DoQuit", 0.5f);    // time to show toast
        } else if (Session.Status.IsError()) {
            _ShowAndroidToastMessage("ARCore encountered a problem connecting.  Please start the app again.");
            m_IsQuitting = true;
            Invoke("_DoQuit", 0.5f);  // time to show error
        }
    }

    // Quit the application
    private void _DoQuit() {
        Application.Quit();
    }

    // Show an Android toast message.
    private void _ShowAndroidToastMessage(string message) {
        AndroidJavaClass unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        AndroidJavaObject unityActivity =
            unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");

        if (unityActivity != null) {
            AndroidJavaClass toastClass = new AndroidJavaClass("android.widget.Toast");
            unityActivity.Call("runOnUiThread", new AndroidJavaRunnable(() => {
                AndroidJavaObject toastObject =
                    toastClass.CallStatic<AndroidJavaObject>(
                        "makeText", unityActivity, message, 0);
                toastObject.Call("show");
            }));
        }
    }
}
