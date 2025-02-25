# Nuclear Weapons Effects Calculator (nuccalc.cpp)

## Overview

This program calculates the effects of nuclear weapon detonations, including thermal radiation, blast effects, initial radiation, and fallout patterns. It uses physical models and data to provide accurate estimations of weapon effects in urban environments.

## What sucks and to-dos:
- Wind speed 0: need to rework this
- Wonky guesswork for long-term effects (adjusted values from Mr. Google)
- Wind direction not considered. Significant high-yield/high-fallout weapons. 

### 1. Weapon Selection Menu
The program starts by presenting a categorized list of nuclear weapons:
- **Historic Weapons**: Famous weapons from nuclear history
- **United States**: Current US arsenal weapons
- **Russia**: Current Russian arsenal weapons
- **China**: Current Chinese arsenal weapons
- **Other Nuclear Powers**: Weapons from UK, France, Israel, India, Pakistan, and North Korea
- **Custom Input**: Option to enter custom yield values

Each weapon entry shows:
- Name/Type: The weapon's name and its type (e.g., "W88/SLBM Thermonuclear")
- Yield: The weapon's yield in megatons (MT)

Enter the number corresponding to your selection.

```
------------------------------------------------------------------------------
Nuclear Weapon Selection
------------------------------------------------------------------------------
Historic Weapons:
1. Little Boy (US)/Uranium Gun-Type (0.015 MT)      2. Fat Man (US)/Plutonium Implosion (0.021 MT)
3. Ivy King (US)/Fission (0.500 MT)                 4. Castle Bravo (US)/Thermonuclear (15.000 MT)
5. Tsar Bomba (USSR)/Thermonuclear (50.000 MT)      
------------------------------------------------------------------------------
United States:
6. W88/SLBM Thermonuclear (0.475 MT)
7. W87/ICBM Thermonuclear (0.300 MT)                8. W76-1/SLBM Thermonuclear (0.100 MT)
9. W78/ICBM Thermonuclear (0.350 MT)                10. B61-12/Variable Yield (0.050 MT)
11. W80/Cruise Missile (0.150 MT)                   12. B83/Strategic Bomb (1.200 MT)
------------------------------------------------------------------------------
Russian Weapons:
13. RS-28 Sarmat/MIRV Thermonuclear (0.800 MT)      14. R-36M2 Voevoda/MIRV Thermonuclear (0.750 MT)
15. RT-2PM2 Topol-M/Thermonuclear (0.550 MT)        16. RSM-56 Bulava/SLBM MIRV (0.150 MT)
17. 9K720 Iskander/Enhanced Radiation (0.050 MT)    18. RS-24 Yars/Mobile ICBM (0.300 MT)
------------------------------------------------------------------------------
Chinese Weapons:
19. DF-5B/MIRV Thermonuclear (0.500 MT)             20. DF-41/Mobile MIRV (0.350 MT)
21. JL-2/SLBM (0.250 MT)                            22. DF-31AG/Mobile ICBM (0.250 MT)
23. DF-26/IRB Thermonuclear (0.150 MT)              24. DF-21/Medium Range (0.300 MT)
------------------------------------------------------------------------------
Other Nuclear Powers:
25. Trident D5/UK SLBM (0.100 MT)                   26. M51/French SLBM (0.150 MT)
27. ASMP-A/French Cruise (0.300 MT)                 28. Jericho III/Israeli IRBM (0.400 MT)
29. Agni-V/Indian ICBM (0.250 MT)                   30. K-15 Sagarika/Indian SLBM (0.200 MT)
31. Shaheen-III/Pakistani MRBM (0.200 MT)           32. Babur/Pakistani Cruise (0.050 MT)
33. Hwasong-15/NK ICBM (0.200 MT)                   34. Hwasong-14/NK ICBM (0.150 MT)
35. Pukguksong-2/NK MRBM (0.050 MT)                 
------------------------------------------------------------------------------
36. Custom Input
------------------------------------------------------------------------------

Select weapon (1-36): 
```

### 2. Burst Type Selection Menu
After selecting a weapon, choose the detonation height profile:

1. **Surface Burst** (Height: 0m)
   - Maximum fallout production
   - Reduced blast radius
   - Best for hardened underground targets

2. **Optimal Air Burst**
   - Best combined blast and thermal effects
   - Reduced fallout
   - Recommended for most targets

3. **Low Air Burst**
   - Balanced effects profile
   - Moderate fallout production
   - Good for mixed target types

4. **High Air Burst**
   - Minimum fallout production
   - Reduced blast effects
   - Best for light structural damage over wide areas

5. **Thermal Optimized**
   - Maximum thermal radiation effects
   - Height optimized for thermal damage
   - Best for igniting fires over large areas

6. **Blast Optimized**
   - Maximum blast wave effects
   - Height optimized for overpressure
   - Best for destroying structures

7. **Custom Height**
   - User-defined burst height
   - Manual height input in meters
   - Allows fine-tuning of effects

```
------------------------------------------------------------------------------
Burst Type Selection
------------------------------------------------------------------------------
Optimal Heights Analysis:
Thermal effects:     102m
Blast effects:       83m
Combined optimum:    92m

Select burst type:
------------------------------------------------------------------------------
1. Surface Burst     | Height: 0m              | Maximum fallout, reduced blast radius
------------------------------------------------------------------------------
2. Optimal Air Burst | Height: 92   m          | Best combined blast/thermal effects
------------------------------------------------------------------------------
3. Low Air Burst     | Height: 64   m          | Balanced effects, moderate fallout
------------------------------------------------------------------------------
4. High Air Burst    | Height: 139  m          | Minimum fallout, reduced effects
------------------------------------------------------------------------------
5. Thermal Optimized | Height: 102  m          | Maximum thermal radiation effects
------------------------------------------------------------------------------
6. Blast Optimized   | Height: 83   m          | Maximum blast wave effects
------------------------------------------------------------------------------
7. Custom Height     | User defined height     | Manual height input
------------------------------------------------------------------------------
Enter selection (1-7):
```

### 3. Target City Selection
Choose from a list of European cities. Each entry shows:
- City name
- Country
- Population in millions

Enter the number corresponding to your target city.

```
------------------------------------------------------------------------------
Target City Selection
------------------------------------------------------------------------------
1 . Amsterdam        Netherlands   Pop: 1.1M
2 . Athens           Greece        Pop: 3.2M
3 . Barcelona        Spain         Pop: 1.6M
4 . Belgrade         Serbia        Pop: 1.7M
5 . Berlin           Germany       Pop: 3.7M
6 . Brussels         Belgium       Pop: 2.1M
7 . Bucharest        Romania       Pop: 2.1M
8 . Budapest         Hungary       Pop: 1.8M
9 . Copenhagen       Denmark       Pop: 0.8M
10. Dublin           Ireland       Pop: 1.4M
11. Graz             Austria       Pop: 0.29M
12. Hamburg          Germany       Pop: 1.9M
13. Helsinki         Finland       Pop: 0.66M
14. Kiev             Ukraine       Pop: 3M
15. Linz             Austria       Pop: 0.21M
16. Lisbon           Portugal      Pop: 2.9M
17. London           UK            Pop: 9M
18. Madrid           Spain         Pop: 3.3M
19. Milan            Italy         Pop: 1.4M
20. Moscow           Russia        Pop: 12.5M
21. Munich           Germany       Pop: 1.5M
22. Oslo             Norway        Pop: 0.7M
23. Paris            France        Pop: 2.2M
24. Prague           Czech Rep.    Pop: 1.3M
25. Rome             Italy         Pop: 4.3M
26. Sofia            Bulgaria      Pop: 1.3M
27. Stockholm        Sweden        Pop: 1M
28. Vienna           Austria       Pop: 1.9M
29. Warsaw           Poland        Pop: 1.8M
30. Zagreb           Croatia       Pop: 0.8M
31. Zurich           Switzerland   Pop: 0.43M
------------------------------------------------------------------------------
Enter city number:
```

### 4. Wind Parameters
Enter the wind speed in kilometers per hour (km/h). This affects:
- Fallout pattern shape and size
- Downwind contamination distance
- Dangerous zone calculations

```
------------------------------------------------------------------------------
Wind Parameters
------------------------------------------------------------------------------
Enter wind speed (km/h):
```

### 5. Results Display
The program will show detailed calculations for:

**Weapon Data**
- Yield in megatons
- Burst type (air/ground)
- Height of burst (if applicable)

**Effects Data** (for each effect type: Thermal, Blast, Radiation)
- Severe effects radius and area
- Moderate effects radius and area
- Light effects radius and area

**Fallout Data**
- Wind speed and direction
- Maximum downwind distance
- Pattern width
- Total danger zone area

**Casualties**
- Estimated fatalities
- Severe injuries
- Light injuries
- Total affected population
- Long-term deaths (1, 5, 10, and 20 years)

### Notes
- All distances are in meters (m) or kilometers (km)
- Areas are in square kilometers (km²)
- Population in millions
- Density in people per km²
- Wind speed in kilometers per hour (km/h)

```
Calculated Effects:
==============================================================================
Weapon Data | Yield: 0.15   MT | Type: Air burst | Height: 159.399m
------------------------------------------------------------------------------
Thermal        | Severe: 561 m      (0.99 km²) | Moderate: 842 m      (2.23 km²) | Light: 1.1 km     (3.97 km²)
------------------------------------------------------------------------------
Blast          | Severe: 1.0 km     (3.55 km²) | Moderate: 1.5 km     (7.98 km²) | Light: 2.3 km     (17.96 km²)
------------------------------------------------------------------------------
Radiation      | Severe: 548 m      (0.98 km²) | Moderate: 823 m      (2.20 km²) | Light: 1.0 km     (3.91 km²)
------------------------------------------------------------------------------
Fallout Data | Wind Speed: 3.00 km/h | Max Distance: 74.61 km
Width: 2.80 km | Fallout Zone: 0.64 km²
------------------------------------------------------------------------------

Estimated Casualties in Paris:
=====================================
Fatalities: 76068
Severe Injuries: 50509
Light Injuries: 46205
Total Casualties: 172783
Long-Term Deaths (1 Year): 9671
Long-Term Deaths (5 Years): 19343
Long-Term Deaths (10 Years): 29014
Long-Term Deaths (20 Years): 38686
==============================================================================
```
---
Note: No claim of accuracy! 
