/// Copyright 2018 - Ben Gavin (ben@virtual-olympus.com)
/// Adapted from works by Bill Earl (https://learn.adafruit.com/users/adafruit_support_bill)

#include <Adafruit_NeoPixel.h>
#include <elapsedMillis.h>

// Implemented Patterns
enum AnimatorPattern { NONE, TEMPO_TRACKER, THEATER_CHASE, BOUNCE, CHASE2 };
enum AnimatorDirection { FORWARD, REVERSE };

class NeoAnimator : public Adafruit_NeoPixel 
{   
  private:
    bool _running;
    int16_t _numColors;

    // Last time (ms) the animation was updated
    elapsedMillis _lastUpdate;

    // an internal 'recursive' call for handling patterns
    // that have an internal 'on complete' they want to call
    void (NeoAnimator::*_internalComplete)();
    
    void FreeColors()
    {
        if (Colors != NULL) 
        { 
            free(Colors); 
            Colors = NULL;
        }
    }

    void CheckInternalComplete()
    {
        _internalComplete = NULL;
    }
    
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }

    // Return color, dimmed by 75% (used by scanner)
    uint32_t DimColor(uint32_t color)
    {
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }
    
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color, bool shouldShow = true)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        
        if (shouldShow)
        {
          show();
        }
    }
    
  public:
    // The pattern being displayed by this animator
    AnimatorPattern Pattern;

    // The (initial) direction of the pattern
    AnimatorDirection Direction;

    // Interval (ms) Between animation updates
    uint32_t Interval;

    // Should the pattern repeat itself?
    bool Repeat;
    
    // The colors currently in the pattern
    uint32_t *Colors;

    // The number of steps in the pattern
    uint16_t TotalSteps;

    // The current step in the pattern
    uint16_t CurrentStep;

    // When animation is complete, call this function
    void (*OnComplete)();

    // Constructor - calls base-class constructor to initialize strip
    NeoAnimator(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
        Colors = NULL;
        Repeat = true;
    }

    // Starts the animation
    void Start()
    {
        if (!_running)
        {
            CurrentStep = Direction == FORWARD ? 0 : TotalSteps;
            _lastUpdate = 0;
            _running = true;
        }
    }

    // Stops the animation
    void Stop(bool callOnComplete = true)
    {
        _running = false;
        if (callOnComplete && OnComplete != NULL)
        {
            OnComplete();
        }
    }

    // Restarts the animation from wherever it stopped
    void Restart()
    {
        _lastUpdate = 0;
        _running = true;
    }

    // Resets the animation to the beginning, does not change run state
    void Reset()
    {
        CurrentStep = Direction == FORWARD ? 0 : TotalSteps;
        ColorSet(0);
        _lastUpdate = 0;
    }
    
    // Reverse direction of the pattern
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            CurrentStep = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            CurrentStep = 0;
        }
    }
    
    // The main update function
    void Update()
    {
        // Do nothing if the animation isn't running
        if (!_running)
        {
            return;
        }
        
        if (_lastUpdate >= Interval)
        {
            _lastUpdate = _lastUpdate - Interval;
            switch (Pattern)
            {
              case TEMPO_TRACKER:
                TempoTrackerUpdate();
                break;

              case THEATER_CHASE:
                TheaterChaseUpdate();
                break;

              case BOUNCE:
                BounceUpdate();
                break;

              case CHASE2:
                Chase2Update();
                break;
                  
              default:
                break;
            }
        }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           CurrentStep++;
           if (CurrentStep >= TotalSteps)
            {
                CurrentStep = 0;
                if (_internalComplete != NULL)
                {
                    (this->*_internalComplete)(); // Call our internal completion callback if set
                }
                else if (OnComplete != NULL)
                {
                    OnComplete(); // call the user completion callback
                }
                _running = Repeat;
                if (!Repeat) { Reset(); } // if we're not repeating, clear the panel
            }
        }
        else // Direction == REVERSE
        {
            --CurrentStep;
            if (CurrentStep <= 0)
            {
                CurrentStep = TotalSteps-1;
                if (_internalComplete != NULL)
                {
                    (this->*_internalComplete)(); // Call our internal completion callback if set
                }
                else if (OnComplete != NULL)
                {
                    OnComplete(); // call the completion callback
                }
                _running = Repeat;
                if (!Repeat) { Reset(); } // if we're not repeating, clear the panel
            }
        }
    }

    void SetColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255) 
    {
        if (index < 0 || index > (_numColors - 1)) { return; } // no-op, we're out of range
        
        // First, pre-multiply our alpha value
        r = (r * alpha / 255) & 0xFF;
        g = (g * alpha / 255) & 0xFF;
        b = (b * alpha / 255) & 0xFF;

        // Then get a properly sorted color value
        Colors[index] = Color(r, g, b);
    }

    void ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255)
    {
      // First, pre-multiply our alpha value
      r = (r * alpha / 255) & 0xFF;
      g = (g * alpha / 255) & 0xFF;
      b = (b * alpha / 255) & 0xFF;

      ColorSet(Color(r,g,b));
    }
    
    /* Tempo Tracker */
  private:
    void TempoTrackerReset()
    {
        // Set everything to Black (off)
        //ColorSet(0x00000000);

        if (OnComplete != NULL)
        {
            OnComplete();
        }
    }

  public:
    // Initialize for a Tempo Tracker
    void InitializeTempoTracker(uint32_t color, uint32_t interval, AnimatorDirection dir = FORWARD, bool repeat = true)
    {
        CheckInternalComplete();
        FreeColors();
        
        Colors = malloc(sizeof(uint32_t) * 1);
        _numColors = 1;
        
        Pattern = TEMPO_TRACKER;
        Interval = interval;
        TotalSteps = numPixels() + 1;
        Colors[0] = color;
        Direction = dir;
        Repeat = repeat;

        //_internalComplete = &TempoTrackerReset;
    }

    void TempoTrackerUpdate()
    {
        uint32_t color = Colors == NULL ? 0x00FFFFFF : Colors[0];

        if (CurrentStep == (TotalSteps -1)) {
          // We are at the fake 'extra' step, so blank out everything, then go to step 1
          ColorSet(0x00000000);
          CurrentStep = 0;
        }
        
        setPixelColor(CurrentStep, color);
        show();
        Increment();
    }

    /* Theater Chase */
    // Initialize for a Theater Chase
    void InitializeTheaterChase(uint32_t *colors, uint16_t nColors, uint32_t interval, uint8_t nBlanks = 2, AnimatorDirection dir = FORWARD, bool repeat = true)
    {
        CheckInternalComplete();
        FreeColors();
        
        Pattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels() + 1;
        Direction = dir;
        Repeat = repeat;
        
        Colors = malloc(sizeof(uint32_t) * (nColors + nBlanks));
        for(int i = 0; i < nColors; i++)
        {
            Colors[i] = colors[i];
        }
        
        if (nBlanks > 0)
        {
            for(int i = 0; i < nBlanks; i++)
            {
                Colors[nColors + i] = 0x00000000;
            }
        }
        
        _numColors = nColors + nBlanks;        
   }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
        for(int i = 0; i < numPixels(); i++)
        {
            int j = CurrentStep > i ? _numColors - (CurrentStep - i) : i - CurrentStep;
            //int j = (CurrentStep + i) % _numColors;
            int32_t color = Colors == NULL ? 0x00FFFFFF : Colors[j];
            
            setPixelColor(i, color);
        }

        show();
        Increment();
    }

    /* Bounce */
    void InitializeBounce(uint32_t *colors, uint16_t nColors, uint32_t interval, AnimatorDirection dir = FORWARD, bool repeat = true)
    {
      CheckInternalComplete();
      FreeColors();

      Pattern = BOUNCE;
      Interval = interval;
      TotalSteps = (numPixels() - 1) * 2 + 3;
      Direction = dir;
      Repeat = repeat;
      
      Colors = malloc(sizeof(uint32_t) * nColors);
      for(int i = 0; i < nColors; i++)
      {
          Colors[i] = colors[i];
      }
      
      _numColors = nColors;         
    }

    void BounceUpdate()
    {
        int totalPixels = numPixels();
        for (int i = 0; i < totalPixels; i++)
        {
          int j = i >= _numColors ? _numColors - 1 : i;
          
          if (i == CurrentStep) // first half of the scan
          {
              //Serial.print(i);
              setPixelColor(i, Colors[j]);
          }
          else if (CurrentStep > totalPixels && i > (TotalSteps - CurrentStep - 3)) // fade to black
          {
              setPixelColor(i, DimColor(DimColor(getPixelColor(i))));
          }
        }
        
        show();
        Increment();
    }

  /* Theater Chase Alt */
  void InitializeChase2(uint32_t *colors, uint16_t nColors, uint32_t interval, AnimatorDirection dir = FORWARD, bool repeat = true)
  {
      CheckInternalComplete();
      FreeColors();
      
      Pattern = CHASE2;
      Interval = interval;
      TotalSteps = numPixels() + nColors + 1;
      Direction = dir;
      Repeat = repeat;
      
      Colors = malloc(sizeof(uint32_t) * (nColors));
      for(int i = 0; i < nColors; i++)
      {
          Colors[i] = colors[i];
      }
  
      _numColors = nColors;        
  }

  void Chase2Update()
  {
      ColorSet(0, false);
    
      for(int i = 0; i < _numColors; i++)
      {
          int pos = CurrentStep - i;
          if (pos < 0 || pos >= numPixels()) { continue; }
          setPixelColor(pos, Colors[i]);
      }
  
      show();
      Increment();
  }

};


