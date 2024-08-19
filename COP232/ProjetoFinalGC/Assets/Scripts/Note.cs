using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor.Build.Content;
using UnityEngine;
using UnityEngine.UI;

public class Note : MonoBehaviour
{
    public float Speed;
    public GameManager _gameManager;
    public bool WasHit;
    [SerializeField] private Color _hitColor;
    public float spawnedTime;
    public float pressedTime;
    private RectTransform _rectTransform;
    private BoxCollider2D _boxCollider;
    
    private void Start()
    {
        _rectTransform = GetComponent<RectTransform>();
        float y = (GameManager.StartY - GameManager.EndY) / _gameManager.lifeTime;
        float newButtonHeight = (y * _gameManager.spawnInterval) + 10;
        _rectTransform.sizeDelta = new Vector2(_rectTransform.sizeDelta.x, newButtonHeight);
        _boxCollider = GetComponent<BoxCollider2D>();
        _boxCollider.size = new Vector2(_boxCollider.size.x, newButtonHeight);
    }
    
    void Update()
    {
        float f = (_gameManager.CurrentTime() - spawnedTime)/_gameManager.lifeTime;
        if (f > 1f)
        {
            if (!WasHit)
            {
                _gameManager.GameOver();
                return;
            }
            else
            {
                Destroy(gameObject);
            }
        }
        //transform.position -= new Vector3(0, Speed * Time.deltaTime, 0);
        _rectTransform.anchoredPosition = Vector3.Lerp(new Vector3(_rectTransform.anchoredPosition.x, GameManager.StartY) ,
            new Vector3(_rectTransform.anchoredPosition.x, GameManager.EndY), f);
    }

    public void OnTriggerEnter2D(Collider2D other)
    {
        if (other.gameObject.CompareTag("Limit"))
        {
            if (!WasHit)
            {
                _gameManager.GameOver();

            }
        }
    }

    public void OnTriggerExit2D(Collider2D other)
    {
        if (other.gameObject.CompareTag("Limit"))
        {
            Destroy(gameObject);
        }
    }

    public void HitNote()
    {
        WasHit = true;
        Image buttonImage = GetComponent<Image>();
        buttonImage.color = _hitColor;
        
        Button button = GetComponent<Button>();
        button.interactable = false;
    }
}