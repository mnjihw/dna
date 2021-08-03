using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NewBehaviourScript : MonoBehaviour
{
    // Start is called before the first frame update
    private float angle = 1;

    private List<LineRenderer> lines = new List<LineRenderer>();

    void Start()
    {
        Debug.Log("Started");



        for(float i = 0; i < 360; i += 22.5f) 
        {
            LineRenderer lineRenderer;

            lineRenderer = new GameObject().AddComponent<LineRenderer>();
            lineRenderer.startWidth = .1f;
            lineRenderer.endWidth = .1f;
            lineRenderer.startColor = Color.black;
            lineRenderer.endColor = Color.black;
            lineRenderer.material = new Material(Shader.Find("Sprites/Default"));
            lineRenderer.transform.position = Vector3.up * 1.1f;

            lineRenderer.useWorldSpace = false;

            lineRenderer.transform.SetParent(transform, false);


            //lineRenderer.SetPosition(0, Vector3.zero);
            lineRenderer.SetPosition(0, new Vector3(0.5f * Mathf.Cos(i * Mathf.Deg2Rad) * 0.7f, 0, 0.5f * Mathf.Sin(i * Mathf.Deg2Rad) * 0.7f));
            lineRenderer.SetPosition(1, new Vector3(0.5f * Mathf.Cos(i * Mathf.Deg2Rad), 0, 0.5f * Mathf.Sin(i * Mathf.Deg2Rad)));

            lines.Add(lineRenderer);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (angle > 0)
        {
            transform.Rotate(0, angle, 0);
            angle -= 0.0002f;
        }
    }
}
