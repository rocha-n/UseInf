using GoogleARCore;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PaintingPlacementGenerator : Manipulator
{
    public Camera FirstPersonCamera;
    public GameObject PaintingPrefab;
    public GameObject ManipulatorPrefab;

    // Returns true if the manipulation can be started for the given gesture
    protected override bool CanStartManipulationForGesture(TapGesture gesture) {
        if (gesture.TargetObject == null) {
            return true;
        }

        return false;
    }

    protected override void OnEndManipulation(TapGesture gesture) {
        if (gesture.WasCancelled) {
            return;
        }

        // If gesture is targeting an existing object, return
        if (gesture.TargetObject != null) {
            return;
        }

        // Raycast against the location the player touched to search for planes.
        TrackableHit hit;
        TrackableHitFlags raycastFilter = TrackableHitFlags.PlaneWithinPolygon;

        if (Frame.Raycast(gesture.StartPosition.x, gesture.StartPosition.y, raycastFilter, out hit)) {
            // Use hit pose and camera pose to check if hittest is from back of the plane
            if ((hit.Trackable is DetectedPlane) && 
                Vector3.Dot(FirstPersonCamera.transform.position - hit.Pose.position, hit.Pose.rotation * Vector3.up) < 0) {
                Debug.Log("Hit at back of the current DetectedPlane");
            } else {
                // Instantiate painting and transform it
                var andyObject = Instantiate(PaintingPrefab, hit.Pose.position, hit.Pose.rotation);
                andyObject.transform.Rotate(270, 180, 0, Space.Self);

                // Instantiate manipulator and make painting a child of the manipulator
                var manipulator = Instantiate(ManipulatorPrefab, hit.Pose.position, hit.Pose.rotation);
                andyObject.transform.parent = manipulator.transform;

                // Create an anchor and make manipulator a child of it
                var anchor = hit.Trackable.CreateAnchor(hit.Pose);
                manipulator.transform.parent = anchor.transform;

                // Select the placed object.
                manipulator.GetComponent<Manipulator>().Select();
            }
        }
    }
}
