/**
 * @file DistanceCalculator.hpp
 * @brief TDD-based distance calculation system for TSPLIB instances
 * 
 * This system provides accurate distance calculations following TSPLIB standards
 * for different distance types (EUC_2D, CEIL_2D, ATT, GEO).
 */

#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>

/**
 * @brief Abstract base class for distance calculators
 */
class DistanceCalculator {
public:
    virtual ~DistanceCalculator() = default;
    
    /**
     * @brief Calculate distance between two points
     * @param x1, y1 Coordinates of first point
     * @param x2, y2 Coordinates of second point
     * @return Distance between the points
     */
    virtual double calculateDistance(double x1, double y1, double x2, double y2) const = 0;
    
    /**
     * @brief Get the type name of this calculator
     * @return Type name string
     */
    virtual std::string getType() const = 0;
};

/**
 * @brief Euclidean distance calculator (EUC_2D)
 * Standard Euclidean distance rounded to nearest integer
 */
class EuclideanDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) const override;
    std::string getType() const override { return "EUC_2D"; }
};

/**
 * @brief Ceiling Euclidean distance calculator (CEIL_2D)
 * Euclidean distance rounded up to next integer
 */
class CeilingDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) const override;
    std::string getType() const override { return "CEIL_2D"; }
};

/**
 * @brief Pseudo-Euclidean distance calculator (ATT)
 * Special distance calculation used in kroA series
 */
class AttDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) const override;
    std::string getType() const override { return "ATT"; }
};

/**
 * @brief Geographical distance calculator (GEO)
 * Calculates distance on Earth's surface using coordinates
 */
class GeographicalDistanceCalculator : public DistanceCalculator {
public:
    double calculateDistance(double x1, double y1, double x2, double y2) const override;
    std::string getType() const override { return "GEO"; }
};

/**
 * @brief Factory for creating distance calculators
 */
class DistanceCalculatorFactory {
public:
    /**
     * @brief Create a distance calculator for the specified type
     * @param type Distance type (EUC_2D, CEIL_2D, ATT, GEO)
     * @return Shared pointer to distance calculator, or nullptr if type is unsupported
     */
    static std::shared_ptr<DistanceCalculator> create(const std::string& type);
    
    /**
     * @brief Get all supported distance types
     * @return Vector of supported type names
     */
    static std::vector<std::string> getSupportedTypes();
};
