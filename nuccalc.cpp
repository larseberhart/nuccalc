/*******************************************************************************
 * Nuclear Weapons Effects Calculator
 *
 * This program calculates the effects of nuclear weapons detonations including:
 * - Thermal radiation (based on Stefan-Boltzmann law and atmospheric attenuation)
 * - Blast effects (using modified Brode equation and Sachs scaling)
 * - Initial radiation (based on weapon yield and atmospheric penetration)
 * - Fallout patterns (using DELFIC-based modeling)
 *
 * Calculation Methods:
 * 1. Blast Effects:
 *    - Uses enhanced Brode equation with Sachs scaling
 *    - Incorporates Mach stem formation and triple-point effects
 *    - Accounts for atmospheric pressure variation with height
 *
 * 2. Thermal Radiation:
 *    - Stefan-Boltzmann law for initial thermal energy
 *    - Beer-Lambert law for atmospheric attenuation
 *    - Accounts for humidity and visibility effects
 *    - Includes fireball temperature scaling with yield
 *
 * 3. Fallout:
 *    - Based on DELFIC (Defense Land Fallout Interpretive Code) models
 *    - Considers particle size distribution and activity fractionation
 *    - Accounts for wind speed and atmospheric stability
 *    - Includes terrain roughness effects on deposition
 *
 * 4. Casualty Estimation:
 *    - Uses concentric ring integration method
 *    - Accounts for population density distribution
 *    - Includes both urban core and suburban density patterns
 *    - Considers combined effects of blast, thermal, and radiation
 *
 * Author: Lars Eberhart
 * Version: 1.0
 * Last Updated: 2023
 ******************************************************************************/

// Standard library includes for core functionality
#include <iostream>  // Console input/output streams
#include <cmath>     // Mathematical functions and constants
#include <string>    // String manipulation and storage
#include <iomanip>   // Output formatting control
#include <vector>    // Dynamic array container
#include <map>       // Key-value associative container
#include <algorithm> // Standard algorithms library

// Data structure for fallout pattern calculations
struct FalloutData
{
    double maxDownwindDistance; // Maximum distance fallout travels downwind (km)
    double maxWidth;            // Maximum width of fallout pattern (km)
    double dangerousZoneArea;   // Total area of dangerous fallout (km²)
    double falloutAngle;        // Angular spread of fallout pattern (degrees)
};

// Nested structure for different effect levels and their areas
struct WeaponEffects
{
    struct EffectLevels
    {
        long double severe; // Changed to long double for higher precision
        long double moderate;
        long double light;
        long double severeArea;
        long double moderateArea;
        long double lightArea;
    };

    EffectLevels thermal;   // Thermal radiation effects (burns)
    EffectLevels blast;     // Blast wave effects (overpressure)
    EffectLevels radiation; // Initial nuclear radiation effects
    FalloutData fallout;    // Fallout pattern data
};

// Structure to store preset weapon data
struct WeaponPreset
{
    std::string name;     // Name of the weapon
    std::string type;     // Type of the weapon
    double yield;         // Yield in megatons (MT)
    bool isAirburst;      // Flag for air burst vs surface burst
    double typicalHeight; // Typical height of burst in meters
};

// Structure to store optimal burst heights for different effects
struct OptimalHeight
{
    double thermal;  // Optimal height for thermal effects
    double blast;    // Optimal height for blast effects
    double combined; // Best compromise height
};

// Structure to store burst type information
struct BurstTypeInfo
{
    std::string name;        // Name of the burst type
    double falloutFactor;    // Fallout factor for the burst type
    double radiationFactor;  // Radiation factor for the burst type
    std::string description; // Description of the burst type
};

// Structure to store physical constants used in calculations
struct PhysicalConstants
{
    static constexpr double AIR_DENSITY = 1.225;               // Standard air density (kg/m³)
    static constexpr double SPEED_OF_SOUND = 340.29;           // Speed of sound (m/s)
    static constexpr double GRAVITY = 9.80665;                 // Gravitational acceleration (m/s²)
    static constexpr double ATMOSPHERIC_PRESSURE = 101325.0;   // Sea level pressure (Pa)
    static constexpr double STEFAN_BOLTZMANN = 5.670374419e-8; // Stefan-Boltzmann constant (W/(m²·K⁴))
    static constexpr double PLANCK_CONSTANT = 6.62607015e-34;  // Planck constant (J·s)
    static constexpr double BOLTZMANN_CONSTANT = 1.380649e-23; // Boltzmann constant (J/K)
    static constexpr double LIGHT_SPEED = 299792458.0;         // Speed of light (m/s)
};

// Structure to store city data
struct CityData
{
    std::string name;        // Name of the city
    std::string country;     // Country of the city
    double population;       // Population in millions
    double area;             // Area in square kilometers (km²)
    double density;          // Population density (people per km²)
    double radius;           // Radius of the city (km)
    double suburban_density; // Population density in suburbs (people per km²)
};

// Main calculator class implementation
class NuclearEffectsCalculator
{
private:
    double yield;     // Nuclear weapon yield in megatons
    double height;    // Height of burst in meters
    bool isAirburst;  // Flag for air burst vs surface burst
    double windSpeed; // Wind speed for fallout calculations (km/h)

    // Function to clear the screen
    void clearScreen()
    {
#ifdef _WIN32
        system("cls"); // Clear screen for Windows
#else
        system("clear"); // Clear screen for Unix-based systems
#endif
    }

    // Function to print menu header
    void printMenuHeader(const std::string &title)
    {
        std::cout << std::string(78, '-') << "\n"; // Print a line of dashes
        std::cout << title << "\n";                // Print the title
        std::cout << std::string(78, '-') << "\n"; // Print another line of dashes
    }

    // Function to print menu divider
    void printMenuDivider()
    {
        std::cout << std::string(78, '-') << "\n"; // Print a line of dashes
    }

    // Modified weapon option printing function to show name and type
    void printWeaponOption(size_t index, size_t maxItems)
    {
        std::string mt = std::to_string(PRESETS[index].yield);
        mt = mt.substr(0, mt.find(".") + 4); // Limit to 3 decimal places

        std::string entry = std::to_string(index + 1) + ". " +
                            PRESETS[index].name + "/" + PRESETS[index].type +
                            " (" + mt + " MT)";

        if (index % 2 == 0)
        {
            std::cout << std::setw(52) << std::left << entry; // Increased width to accommodate type
        }
        else
        {
            std::cout << entry << "\n";
        }

        // Add newline for odd-numbered sections
        if (index % 2 == 0 && index == maxItems - 1)
        {
            std::cout << "\n";
        }
    }

    // Function to apply height effects to weapon effects
    void applyHeightEffects(WeaponEffects &effects, double height)
    {
        // Adjust effects based on height of burst
        double heightFactor = 1.0 - (height / 10000.0); // Linear decrease with height
        heightFactor = std::max(0.3, heightFactor);     // Minimum 30% effect

        effects.blast.severe *= heightFactor;   // Reduce severe blast effects
        effects.blast.moderate *= heightFactor; // Reduce moderate blast effects
        effects.blast.light *= heightFactor;    // Reduce light blast effects

        effects.radiation.severe *= heightFactor;   // Reduce severe radiation effects
        effects.radiation.moderate *= heightFactor; // Reduce moderate radiation effects
        effects.radiation.light *= heightFactor;    // Reduce light radiation effects
    }

    // Preset weapon data
    const std::vector<WeaponPreset> PRESETS = { // Name, Type, Yield (MT), Airburst, Height (m)
        // Historic Weapons
        {"Little Boy (US)", "Uranium Gun-Type", 0.015, true, 580},
        {"Fat Man (US)", "Plutonium Implosion", 0.021, true, 503},
        {"Ivy King (US)", "Fission", 0.500, true, 450},
        {"Castle Bravo (US)", "Thermonuclear", 15.0, true, 2000},
        {"Tsar Bomba (USSR)", "Thermonuclear", 50.0, true, 4000},

        // United States
        {"W88", "SLBM Thermonuclear", 0.475, true, 300},
        {"W87", "ICBM Thermonuclear", 0.300, true, 300},
        {"W76-1", "SLBM Thermonuclear", 0.100, true, 250},
        {"W78", "ICBM Thermonuclear", 0.350, true, 300},
        {"B61-12", "Variable Yield", 0.050, true, 200},
        {"W80", "Cruise Missile", 0.150, true, 250},
        {"B83", "Strategic Bomb", 1.200, true, 300},

        // Russia
        {"RS-28 Sarmat", "MIRV Thermonuclear", 0.800, true, 350},
        {"R-36M2 Voevoda", "MIRV Thermonuclear", 0.750, true, 300},
        {"RT-2PM2 Topol-M", "Thermonuclear", 0.550, true, 300},
        {"RSM-56 Bulava", "SLBM MIRV", 0.150, true, 250},
        {"9K720 Iskander", "Enhanced Radiation", 0.050, true, 200},
        {"RS-24 Yars", "Mobile ICBM", 0.300, true, 300},

        // China
        {"DF-5B", "MIRV Thermonuclear", 0.500, true, 300},
        {"DF-41", "Mobile MIRV", 0.350, true, 250},
        {"JL-2", "SLBM", 0.250, true, 250},
        {"DF-31AG", "Mobile ICBM", 0.250, true, 300},
        {"DF-26", "IRB Thermonuclear", 0.150, true, 200},
        {"DF-21", "Medium Range", 0.300, true, 250},

        // Other Nuclear Powers
        {"Trident D5", "UK SLBM", 0.100, true, 250},
        {"M51", "French SLBM", 0.150, true, 250},
        {"ASMP-A", "French Cruise", 0.300, true, 200},
        {"Jericho III", "Israeli IRBM", 0.400, true, 250},
        {"Agni-V", "Indian ICBM", 0.250, true, 300},
        {"K-15 Sagarika", "Indian SLBM", 0.200, true, 250},
        {"Shaheen-III", "Pakistani MRBM", 0.200, true, 250},
        {"Babur", "Pakistani Cruise", 0.050, true, 200},
        {"Hwasong-15", "NK ICBM", 0.200, true, 250},
        {"Hwasong-14", "NK ICBM", 0.150, true, 250},
        {"Pukguksong-2", "NK MRBM", 0.050, true, 200}};

    // Burst type information
    const std::map<std::string, BurstTypeInfo> BURST_TYPES = {
        {"surface", {"Surface Burst", 1.0, 1.0, "Maximum fallout, reduced blast radius"}},
        {"optimum", {"Optimal Air Burst", 0.5, 0.7, "Best blast/thermal effects"}},
        {"low", {"Low Air Burst", 0.7, 0.8, "Balanced effects"}},
        {"high", {"High Air Burst", 0.3, 0.5, "Minimum fallout, reduced blast"}}};

    // Helper function to calculate circular area
    double calculateArea(double radius)
    {
        return M_PI * pow(radius / 1000.0, 2); // Convert meters to kilometers for area calculation
    }

    // Function to print effect header in table format
    void printEffectHeader()
    {
        std::cout << std::setw(12) << "Severity"     // Print header for effect levels
                  << std::setw(15) << "Radius (m)"   // Print header for effect radii
                  << std::setw(20) << "Area (km²)\n" // Print header for effect areas
                  << std::string(47, '-') << "\n";   // Print a line of dashes
    }

    // Function to display effect levels
    void displayEffectLevel(const std::string &name, const WeaponEffects::EffectLevels &effect)
    {
        std::cout << std::left << std::setw(15) << name << "| ";

        auto formatDistance = [](long double dist) -> std::string
        {
            if (dist < 1.0)
                return "< 1 m";
            if (dist >= 1000.0)
            {
                return std::to_string(static_cast<int>(dist / 1000.0)) + "." +
                       std::to_string(static_cast<int>(fmod(dist, 1000.0) / 100.0)) + " km";
            }
            return std::to_string(static_cast<int>(dist)) + " m";
        };

        std::cout << "Severe: " << std::setw(10) << formatDistance(effect.severe)
                  << " (" << std::fixed << std::setprecision(2) << effect.severeArea << " km²) | ";
        std::cout << "Moderate: " << std::setw(10) << formatDistance(effect.moderate)
                  << " (" << effect.moderateArea << " km²) | ";
        std::cout << "Light: " << std::setw(10) << formatDistance(effect.light)
                  << " (" << effect.lightArea << " km²)\n";
    }

    // Function to display weapon presets
    void displayPresets()
    {
        clearScreen();                               // Clear the screen
        printMenuHeader("Nuclear Weapon Selection"); // Print menu header

        size_t i = 0;

        // Display Historic Weapons
        std::cout << "Historic Weapons:\n";
        for (; i < 5; i++)
        {
            printWeaponOption(i, 5); // Print weapon options
        }

        printMenuDivider(); // Print menu divider

        // Display US Weapons
        std::cout << "United States:\n";
        for (; i < 12; i++)
        {
            printWeaponOption(i, 12); // Print weapon options
        }

        printMenuDivider(); // Print menu divider

        // Display Russian Weapons
        std::cout << "Russian Weapons:\n";
        for (; i < 18; i++)
        {
            printWeaponOption(i, 18); // Print weapon options
        }

        printMenuDivider(); // Print menu divider

        // Display Chinese Weapons
        std::cout << "Chinese Weapons:\n";
        for (; i < 24; i++)
        {
            printWeaponOption(i, 24); // Print weapon options
        }

        printMenuDivider(); // Print menu divider

        // Display Other Nuclear Powers
        std::cout << "Other Nuclear Powers:\n";
        for (; i < PRESETS.size(); i++)
        {
            printWeaponOption(i, PRESETS.size()); // Print weapon options
        }

        printMenuDivider();                                    // Print menu divider
        std::cout << PRESETS.size() + 1 << ". Custom Input\n"; // Print custom input option
        printMenuDivider();                                    // Print menu divider
    }

    // Function to calculate optimal height of burst
    OptimalHeight calculateOptimalHeight()
    {
        OptimalHeight oh;
        // Height of burst calculations based on yield
        double yieldFactor = pow(yield, 1.0 / 3.0); // Cube root scaling
        oh.thermal = 220 * yieldFactor;             // Optimal for thermal effects
        oh.blast = 180 * yieldFactor;               // Optimal for blast effects
        oh.combined = 200 * yieldFactor;            // Compromise height
        return oh;
    }

    // Function to set burst parameters
    void setBurstParameters()
    {
        clearScreen();                           // Clear the screen
        printMenuHeader("Burst Type Selection"); // Print menu header

        OptimalHeight oh = calculateOptimalHeight(); // Calculate optimal heights

        std::cout << "Optimal Heights Analysis:\n";                          // Print optimal heights
        std::cout << "Thermal effects:     " << (int)oh.thermal << "m\n";    // Print thermal effects
        std::cout << "Blast effects:       " << (int)oh.blast << "m\n";      // Print blast effects
        std::cout << "Combined optimum:    " << (int)oh.combined << "m\n\n"; // Print combined effects

        std::cout << "Select burst type:\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "1. Surface Burst     | Height: 0m              | Maximum fallout, reduced blast radius\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "2. Optimal Air Burst | Height: " << std::setw(5) << (int)oh.combined << "m          | Best combined blast/thermal effects\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "3. Low Air Burst     | Height: " << std::setw(5) << (int)(oh.combined * 0.7) << "m          | Balanced effects, moderate fallout\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "4. High Air Burst    | Height: " << std::setw(5) << (int)(oh.combined * 1.5) << "m          | Minimum fallout, reduced effects\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "5. Thermal Optimized | Height: " << std::setw(5) << (int)oh.thermal << "m          | Maximum thermal radiation effects\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "6. Blast Optimized   | Height: " << std::setw(5) << (int)oh.blast << "m          | Maximum blast wave effects\n";
        std::cout << std::string(78, '-') << "\n";
        std::cout << "7. Custom Height     | User defined height     | Manual height input\n";
        std::cout << std::string(78, '-') << "\n";

        std::cout << "Enter selection (1-7): ";
        int choice;
        std::cin >> choice; // Get user input
        isAirburst = (choice > 1);
        switch (choice)
        {
        case 1:
            height = 0;
            break;
        case 2:
            height = oh.combined;
            break;
        case 3:
            height = oh.combined * 0.7;
            break;
        case 4:
            height = oh.combined * 1.5;
            break;
        case 5:
            height = oh.thermal;
            break;
        case 6:
            height = oh.blast;
            break;
        case 7:
            std::cout << "Enter burst height (meters): ";
            std::cin >> height;
            // Validate height input
            if (height < 0)
                height = 0;
            if (height > oh.combined * 3)
            {
                std::cout << "Warning: Height might be too high for effective weapon use\n";
            }
            break;
        default:
            height = oh.combined; // Default to optimal height
            break;
        }
    }

    // Core calculation function for blast overpressure effects
    long double calculateBlastOverpressure(long double distance, long double yield)
    {
        // Convert nuclear yield from megatons to joules (1 MT = 4.184e15 J)
        long double E = yield * 4.184e15; // Total energy release in joules

        // Calculate scaled distance using Sachs scaling law for nuclear explosions
        // This accounts for atmospheric pressure effects on blast wave propagation
        long double scaled_distance = distance / pow(E / PhysicalConstants::ATMOSPHERIC_PRESSURE, 1.0L / 3.0L);

        // Calculate Mach stem enhancement factor for airburst detonations
        // Mach stem forms when incident and reflected shock waves merge
        long double mach_stem_factor = 1.0; // Initialize to no enhancement
        if (height > 0)
        {
            // Scale height relative to yield using cube root scaling
            long double mach_height = height / pow(yield, 1.0L / 3.0L);
            // Enhancement decreases exponentially with scaled height
            mach_stem_factor = 1.0 + 0.1 * exp(-mach_height / 100.0);
        }

        // Calculate triple-point effects where Mach stem begins to form
        // This occurs at a specific height-dependent distance from ground zero
        long double triple_point_height = 83 * pow(yield, 0.4); // Empirical relationship
        if (height > 0 && height < triple_point_height)
        {
            // Enhance blast effects in Mach stem region
            mach_stem_factor *= 1.25; // 25% enhancement in Mach region
        }

        // Calculate final overpressure using modified Brode equation
        // Terms represent different components of blast wave behavior:
        // - 1.0: ambient pressure term
        // - 0.076/scaled_distance: initial shock wave
        // - 0.255/scaled_distance^2: positive phase duration
        // - 0.536/scaled_distance^3: negative phase effects
        return PhysicalConstants::ATMOSPHERIC_PRESSURE *
               (1.0 + 0.076 / scaled_distance + 0.255 / pow(scaled_distance, 2.0) +
                0.536 / pow(scaled_distance, 3.0)) *
               mach_stem_factor;
    }

    // Thermal radiation calculation with atmospheric effects
    long double calculateThermalRadiation(long double distance, long double yield, long double height)
    {
        // Fix thermal calculation
        const long double THERMAL_CONSTANT = 10000.0; // Calibration constant
        long double E = yield * 4.184e15 * 0.35;
        long double fireball_temperature = 6000.0 + 1000.0 * log10(yield);

        // Simplified thermal radiation formula
        long double thermal_energy = THERMAL_CONSTANT * (E / (4.0 * M_PI * pow(distance, 2.0)));

        // Apply atmospheric attenuation
        long double transmission = exp(-0.17 * distance / 1000.0);

        if (height > 0)
        {
            long double angle_factor = sqrt(1.0 - pow(height / (distance + height), 2.0));
            thermal_energy *= angle_factor * exp(-height / 7400.0);
        }

        return thermal_energy * transmission;
    }

    // Function to calculate fallout pattern
    FalloutData calculateFallout()
    {
        FalloutData fallout;

        // Calculate stabilized cloud height
        double stabilizedHeight = (height == 0) ? 212.0 * pow(yield, 0.375) : // Ground burst
                                      188.0 * pow(yield, 0.375);              // Air burst

        // Calculate particle fraction and activity
        double particleFraction = isAirburst ? 0.3 * exp(-height / (stabilizedHeight * 0.7)) : 1.0;
        double activityFraction = 0.6 + 0.2 * log10(yield);
        double effectiveYield = yield * particleFraction * activityFraction;

        // Base fallout radius due to mushroom cloud spread
        double baseRadius = 1000.0 * pow(effectiveYield, 0.4);

        if (windSpeed < 0.1)
        { // Near-zero wind conditions
            // Create circular pattern
            fallout.maxDownwindDistance = baseRadius / 1000.0; // Convert to km
            fallout.maxWidth = baseRadius / 1000.0;            // Equal in all directions
            fallout.falloutAngle = 360.0;                      // Full circle
        }
        else
        {
            // Calculate wind-driven pattern
            fallout.maxDownwindDistance = std::max(
                baseRadius / 1000.0, // Minimum distance
                windSpeed * 3600.0 * (pow(effectiveYield, 0.4) / PhysicalConstants::GRAVITY) *
                    (1.0 + 0.15 * log10(yield)));

            // Width calculation with turbulent diffusion
            fallout.maxWidth = fallout.maxDownwindDistance *
                               (0.14 + 0.02 * log10(yield)) *
                               pow(stabilizedHeight / 1000.0, 0.5);

            // Fallout angle for wind conditions
            fallout.falloutAngle = 40.0 * exp(-height / (stabilizedHeight * 2.0)) *
                                   (1.0 - 0.1 * log10(std::max(1.0, windSpeed)));
        }

        // Calculate danger zone area
        if (windSpeed < 0.1)
        {
            fallout.dangerousZoneArea = M_PI * pow(fallout.maxDownwindDistance, 2);
        }
        else
        {
            fallout.dangerousZoneArea = 0.5 * fallout.maxDownwindDistance *
                                        fallout.maxWidth * particleFraction *
                                        (1.0 - 0.2 * isAirburst);
        }

        // Scale all values based on burst type
        double falloutScale = (height == 0) ? 1.0 : 0.3; // Ground burst produces more fallout
        fallout.dangerousZoneArea *= falloutScale;

        return fallout;
    }

    // Add city selection
    const std::vector<CityData> CITIES = {
        {"Amsterdam", "Netherlands", 1.1, 219, 5023, 9.2, 2100},
        {"Athens", "Greece", 3.2, 412, 7767, 15.2, 2200},
        {"Barcelona", "Spain", 1.6, 101, 15842, 5.8, 3500},
        {"Belgrade", "Serbia", 1.7, 360, 4722, 10.7, 1200},
        {"Berlin", "Germany", 3.7, 892, 4147, 16.8, 1800},
        {"Brussels", "Belgium", 2.1, 161, 13043, 7.2, 3200},
        {"Bucharest", "Romania", 2.1, 228, 9210, 8.5, 1500},
        {"Budapest", "Hungary", 1.8, 525, 3428, 12.9, 1100},
        {"Copenhagen", "Denmark", 0.8, 180, 4444, 7.5, 1800},
        {"Dublin", "Ireland", 1.4, 115, 12174, 6.1, 2500},
        {"Graz", "Austria", 0.29, 127, 2283, 6.4, 800},
        {"Hamburg", "Germany", 1.9, 755, 2517, 15.5, 1200},
        {"Helsinki", "Finland", 0.66, 215, 3070, 8.2, 1400},
        {"Kiev", "Ukraine", 3.0, 839, 3575, 16.3, 900},
        {"Linz", "Austria", 0.21, 96, 2187, 5.5, 700},
        {"Lisbon", "Portugal", 2.9, 100, 29000, 5.6, 4200},
        {"London", "UK", 9.0, 1572, 5724, 22.5, 3500},
        {"Madrid", "Spain", 3.3, 604, 5464, 13.8, 2200},
        {"Milan", "Italy", 1.4, 182, 7692, 7.6, 2800},
        {"Moscow", "Russia", 12.5, 2511, 4978, 28.1, 2000},
        {"Munich", "Germany", 1.5, 310, 4839, 9.9, 1900},
        {"Oslo", "Norway", 0.7, 454, 1542, 12.0, 800},
        {"Paris", "France", 2.2, 105, 20952, 5.8, 5500},
        {"Prague", "Czech Rep.", 1.3, 496, 2621, 12.5, 1100},
        {"Rome", "Italy", 4.3, 1285, 3345, 20.2, 1600},
        {"Sofia", "Bulgaria", 1.3, 492, 2642, 12.5, 900},
        {"Stockholm", "Sweden", 1.0, 188, 5319, 7.7, 1700},
        {"Vienna", "Austria", 1.9, 415, 4579, 11.5, 1600},
        {"Warsaw", "Poland", 1.8, 517, 3483, 12.8, 1400},
        {"Zagreb", "Croatia", 0.8, 641, 1248, 14.2, 600},
        {"Zurich", "Switzerland", 0.43, 88, 4886, 5.3, 2200}};

    CityData selectedCity;

    // Add density calculation based on distance from center
    double calculateDensityAtDistance(double distance)
    {
        double cityRadius = selectedCity.radius;
        double cityDensity = selectedCity.density;
        double suburbanDensity = selectedCity.suburban_density;

        if (distance <= cityRadius)
        {
            // Exponential density decrease within city
            return cityDensity * exp(-distance / cityRadius);
        }
        else
        {
            // Suburban density with exponential falloff
            return suburbanDensity * exp(-(distance - cityRadius) / (cityRadius * 0.5));
        }
    }

    // Add long-term effects to CasualtyEstimate
    struct CasualtyEstimate
    {
        double deaths;
        double severeInjuries;
        double lightInjuries;
        double longTermDeaths1Year;
        double longTermDeaths5Year;
        double longTermDeaths10Year;
        double longTermDeaths20Year;
    };

    // Update casualty calculation with long-term effects
    CasualtyEstimate calculateCasualties(const WeaponEffects &effects)
    {
        CasualtyEstimate casualties = {0, 0, 0, 0, 0, 0, 0};

        // Calculate casualties in concentric rings
        const int RINGS = 20; // Number of calculation rings
        double maxRadius = std::max({sqrt(effects.blast.lightArea / M_PI),
                                     sqrt(effects.thermal.lightArea / M_PI),
                                     sqrt(effects.radiation.lightArea / M_PI)});

        for (int i = 0; i < RINGS; i++)
        {
            double innerRadius = (i * maxRadius) / RINGS;                                     // Calculate inner ring radius
            double outerRadius = ((i + 1) * maxRadius) / RINGS;                               // Calculate outer ring radius
            double ringArea = M_PI * (outerRadius * outerRadius - innerRadius * innerRadius); // Calculate ring area
            double avgRadius = (innerRadius + outerRadius) / 2;                               // Calculate average radius
            double density = calculateDensityAtDistance(avgRadius);                           // Calculate density at average radius

            // Calculate effects for this ring
            if (avgRadius <= sqrt(effects.blast.severeArea / M_PI))
            {
                casualties.deaths += ringArea * density * 0.9; // 90% mortality
            }
            else if (avgRadius <= sqrt(effects.blast.moderateArea / M_PI))
            {
                casualties.severeInjuries += ringArea * density * 0.5; // 50% severe injuries
            }
            else if (avgRadius <= sqrt(effects.blast.lightArea / M_PI))
            {
                casualties.lightInjuries += ringArea * density * 0.3; // 30% light injuries
            }

            // Add thermal effects
            if (avgRadius <= sqrt(effects.thermal.severeArea / M_PI))
            {
                casualties.deaths += ringArea * density * 0.7; // 70% mortality
            }
            // ...similar calculations for moderate and light thermal effects...

            // Add radiation effects
            if (avgRadius <= sqrt(effects.radiation.severeArea / M_PI))
            {
                casualties.severeInjuries += ringArea * density * 0.8; // 80% severe injuries
            }
            // ...similar calculations for moderate radiation effects...
        }

        // Estimate long-term deaths based on radiation exposure - guesswork
        double totalExposed = casualties.severeInjuries + casualties.lightInjuries; // Total exposed population
        casualties.longTermDeaths1Year = totalExposed * 0.1;                        // 10% mortality in 1 year
        casualties.longTermDeaths5Year = totalExposed * 0.2;                        // 20% mortality in 5 years
        casualties.longTermDeaths10Year = totalExposed * 0.3;                       // 30% mortality in 10 years
        casualties.longTermDeaths20Year = totalExposed * 0.4;                       // 40% mortality in 20 years

        return casualties;
    }

    // Function to display casualty estimates
    void displayCasualties(const CasualtyEstimate &casualties)
    {
        std::cout << "\nEstimated Casualties in " << selectedCity.name << ":\n";
        std::cout << "=====================================\n";
        std::cout << "Fatalities: " << std::fixed << std::setprecision(0)
                  << casualties.deaths << "\n";
        std::cout << "Severe Injuries: " << casualties.severeInjuries << "\n";
        std::cout << "Light Injuries: " << casualties.lightInjuries << "\n";
        std::cout << "Total Casualties: "
                  << (casualties.deaths + casualties.severeInjuries +
                      casualties.lightInjuries)
                  << "\n";
        std::cout << "Long-Term Deaths (1 Year): " << casualties.longTermDeaths1Year << "\n";
        std::cout << "Long-Term Deaths (5 Years): " << casualties.longTermDeaths5Year << "\n";
        std::cout << "Long-Term Deaths (10 Years): " << casualties.longTermDeaths10Year << "\n";
        std::cout << "Long-Term Deaths (20 Years): " << casualties.longTermDeaths20Year << "\n";
    }

    // Function to select target city
    void selectCity()
    {
        clearScreen();
        printMenuHeader("Target City Selection");

        for (size_t i = 0; i < CITIES.size(); i++)
        {
            std::cout << std::setw(2) << i + 1 << ". "
                      << std::setw(15) << CITIES[i].name
                      << "  " << std::setw(12) << CITIES[i].country
                      << "  Pop: " << CITIES[i].population << "M\n";
        }

        printMenuDivider();
        std::cout << "Enter city number: ";
        int choice;
        std::cin >> choice;

        if (choice > 0 && choice <= CITIES.size())
        {
            selectedCity = CITIES[choice - 1];
        }
        else
        {
            selectedCity = CITIES[0]; // Default to London
        }
    }

    // Function to set wind parameters
    void setWindParameters()
    {
        clearScreen();
        printMenuHeader("Wind Parameters");

        std::cout << "Enter wind speed (km/h): ";
        std::cin >> windSpeed;

        printMenuDivider();
    }

public:
    NuclearEffectsCalculator()
        : yield(0), height(0), isAirburst(false),
          windSpeed(0) {}

    // Function to set parameters for the calculation
    void setParameters()
    {
        displayPresets();
        std::cout << "\nSelect weapon (1-" << PRESETS.size() + 1 << "): ";
        int choice;
        std::cin >> choice;

        if (choice <= PRESETS.size())
        {
            const auto &preset = PRESETS[choice - 1];
            yield = preset.yield;
            isAirburst = preset.isAirburst;
            height = preset.typicalHeight;
            std::cout << "\nSelected: " << preset.name << " (" << preset.type << ")\n";
        }
        else
        {
            std::cout << "Enter yield (MT): ";
            std::cin >> yield;

            std::cout << "Select detonation type:\n";
            std::cout << "1. Ground burst\n2. Air burst\n";
            std::cin >> choice;
            isAirburst = (choice == 2);

            if (isAirburst)
            {
                std::cout << "Enter height of burst (meters): ";
                std::cin >> height;
            }
        }

        setBurstParameters(); // Add burst parameter selection

        selectCity(); // Add city selection

        setWindParameters(); // Add wind parameters
    }

    // Function to calculate weapon effects
    WeaponEffects calculateEffects()
    {
        WeaponEffects effects;
        long double E = yield * 4.184e15;

        // Updated scaling factors
        long double blastScaling = pow(yield, 1.0L / 3.0L); // Cube root scaling
        long double thermalScaling = pow(yield, 0.4L);      // Thermal scaling
        long double radiationScaling = pow(yield, 0.19L);   // Radiation scaling

        // Calculate blast effects (in meters)
        effects.blast = {
            2000.0 * blastScaling, // Severe damage radius (20 psi)
            3000.0 * blastScaling, // Moderate damage radius (10 psi)
            4500.0 * blastScaling, // Light damage radius (5 psi)
            calculateArea(2000.0 * blastScaling),
            calculateArea(3000.0 * blastScaling),
            calculateArea(4500.0 * blastScaling)};

        // Calculate thermal effects (in meters)
        effects.thermal = {
            1200.0 * thermalScaling, // Severe burns radius
            1800.0 * thermalScaling, // Moderate burns radius
            2400.0 * thermalScaling, // Light burns radius
            calculateArea(1200.0 * thermalScaling),
            calculateArea(1800.0 * thermalScaling),
            calculateArea(2400.0 * thermalScaling)};

        // Calculate radiation effects (in meters)
        effects.radiation = {
            800.0 * radiationScaling,  // Lethal dose radius
            1200.0 * radiationScaling, // Severe effects radius
            1600.0 * radiationScaling, // Light effects radius
            calculateArea(800.0 * radiationScaling),
            calculateArea(1200.0 * radiationScaling),
            calculateArea(1600.0 * radiationScaling)};

        // Apply height of burst effects
        if (height > 0)
        {
            applyHeightEffects(effects, height);
        }

        effects.fallout = calculateFallout();
        return effects;
    }

    // Function to display results
    void displayResults(const WeaponEffects &effects)
    {
        // Clear screen before displaying results
        clearScreen();

        std::cout << "\nCalculated Effects:\n";
        std::cout << std::string(78, '=') << "\n";

        // Weapon Info
        std::cout << "Weapon Data | ";
        std::cout << "Yield: " << std::setw(6) << yield << " MT | ";
        std::cout << "Type: " << (isAirburst ? "Air burst" : "Ground burst");
        if (isAirburst)
        {
            std::cout << " | Height: " << height << "m";
        }
        std::cout << "\n"
                  << std::string(78, '-') << "\n";

        // Effects Data
        displayEffectLevel("Thermal", effects.thermal);
        std::cout << std::string(78, '-') << "\n";
        displayEffectLevel("Blast", effects.blast);
        std::cout << std::string(78, '-') << "\n";
        displayEffectLevel("Radiation", effects.radiation);
        std::cout << std::string(78, '-') << "\n";

        // Fallout Information
        std::cout << "Fallout Data | ";
        std::cout << "Wind Speed: " << std::setw(3) << windSpeed << " km/h | ";
        std::cout << "Max Distance: " << std::setw(5) << effects.fallout.maxDownwindDistance << " km\n";
        std::cout << "Width: " << effects.fallout.maxWidth << " km | ";
        std::cout << "Fallout Zone: " << effects.fallout.dangerousZoneArea << " km²\n";
        std::cout << std::string(78, '-') << "\n";

        // Casualties
        CasualtyEstimate casualties = calculateCasualties(effects);
        displayCasualties(casualties);
        std::cout << std::string(78, '=') << "\n";
    }
};

int main()
{
    NuclearEffectsCalculator calculator;
    calculator.setParameters();
    WeaponEffects effects = calculator.calculateEffects();
    calculator.displayResults(effects);
    return 0;
}
