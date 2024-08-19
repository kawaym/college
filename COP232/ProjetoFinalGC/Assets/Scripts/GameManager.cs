using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.UI;
using Random = UnityEngine.Random;

public class GameManager : MonoBehaviour
{
    [SerializeField] private GameObject buttonPrefab;
    [SerializeField] private Transform buttonParent;
    [SerializeField] private GameObject _gameOverScreen;
    private int _points;
    //public List<GameObject> buttonList = new List<GameObject>();
    [SerializeField] private TMP_Text _pointsText;
    [SerializeField] private TMP_Text _gameOverPoints;
    [SerializeField] public float BPM;
    [SerializeField] private AudioSource _audioSource;
    [SerializeField] private List<Color> _buttonColors;
    private float timer = 0f;
    private bool _isPlaying = true;
    public float lifeTime;
    private List<Vector3> _buttonPositions = new List<Vector3>
    {
        new Vector3(-405, 1185, 0),
        new Vector3(-135, 1185, 0),
        new Vector3(135, 1185, 0),
        new Vector3(405, 1185, 0)
    };
    
    
    public float spawnInterval; // Intervalo de tempo em segundos entre cada spawn de nota


    public static float StartY = 1185f;
    public static float EndY = -835f;

    private void Awake()
    {
        spawnInterval = 60f / BPM;
    }

    void Start()
    {
        StartNewGame();
    }

    public void Update()
    {
        if (!_isPlaying) return;
        timer += Time.deltaTime;
        if (timer >= spawnInterval)
        {
            int randomIndex = Random.Range(0, 4);
            AddButton(randomIndex);
            timer = 0f;
        }
    }

    public void StartNewGame()
    {
        _points = 0;
        _gameOverScreen.SetActive(false);
        foreach(Transform child in buttonParent)
        {
            Destroy(child.gameObject);
        }
        _audioSource.Play();
        _isPlaying = true;
        timer = 0f;
    }
    public void AddButton(int index) 
    {
        GameObject newButton = Instantiate(buttonPrefab, buttonParent);
        newButton.GetComponent<Button>().onClick.AddListener(() => UpdatePoints(newButton));
        //buttonList.Add(newButton);
        newButton.GetComponent<Image>().color = _buttonColors[index];
        RectTransform rectTransform = newButton.GetComponent<RectTransform>();
        rectTransform.anchoredPosition = _buttonPositions[index];
        
        Note note = newButton.GetComponent<Note>();
        note._gameManager = this;
        note.spawnedTime = CurrentTime();
    }
    public void UpdatePoints(GameObject button)
    {
        _points += 1;
        _pointsText.text = _points.ToString();
        button.GetComponent<Note>().HitNote();
    }

    public void GameOver()
    {
        _isPlaying = false;
        //foreach (var button in buttonList)
        //{
        //    buttonList.Remove(button);
        //    Destroy(button);
        //}
        _audioSource.Stop();
        _gameOverScreen.SetActive(true);
        _gameOverPoints.text = _points.ToString();
    }

    public float CurrentTime()
    {
        return _audioSource.time;
    }
    
}
