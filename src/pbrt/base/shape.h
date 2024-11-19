// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_BASE_SHAPE_H
#define PBRT_BASE_SHAPE_H

#include <pbrt/pbrt.h>

#include <pbrt/base/texture.h>
#include <pbrt/util/taggedptr.h>
#include <pbrt/util/vecmath.h>

#include <map>
#include <string>

namespace pbrt {

// Shape Declarations
class Triangle;
class BilinearPatch;
class Curve;
class Sphere;
class Cylinder;
class Disk;

struct ShapeSample;
struct ShapeIntersection;
struct ShapeSampleContext;

// Shape Definition
class Shape
    : public TaggedPointer<Sphere, Cylinder, Disk, Triangle, BilinearPatch, Curve> {
  public:
    // Shape Interface
    using TaggedPointer::TaggedPointer;

    static pstd::vector<Shape> Create(
        const std::string &name, const Transform *renderFromObject,
        const Transform *objectFromRender, bool reverseOrientation,
        const ParameterDictionary &parameters,
        const std::map<std::string, FloatTexture> &floatTextures, const FileLoc *loc,
        Allocator alloc);
    std::string ToString() const;

    PBRT_CPU_GPU inline Bounds3f Bounds() const;

    PBRT_CPU_GPU inline DirectionCone NormalBounds() const;

    PBRT_CPU_GPU inline pstd::optional<ShapeIntersection> Intersect(
        const Ray &ray, Float tMax = Infinity) const;

    PBRT_CPU_GPU inline bool IntersectP(const Ray &ray, Float tMax = Infinity) const;

    PBRT_CPU_GPU inline Float Area() const;

    /*
       Necessary to sample points on the surface of shapes in order to use them as
       emitters. 
       There are two shape sampling methods, both named Sample().

    */

    /*
       The first chooses
       points on the surface of the shape using a sampling distribution with respect to
       surface area and returns the local geometric information about the sampled point in
       a ShapeSample.
       
       The provided sample value u, a uniform sample in [0-1] , should be used to
       determine the point on the shape.
       
    */
    PBRT_CPU_GPU inline pstd::optional<ShapeSample> Sample(Point2f u) const;

    PBRT_CPU_GPU inline Float PDF(const Interaction &) const;

    /*
        The second shape sampling method takes a reference point from which the shape is being viewed. 
        This method is particularly useful for lighting, since the caller can pass in the point to 
        be lit and allow shape implementations to ensure that they only sample the portion of the shape that is potentially visible from that point.
        Unlike the first Shape sampling method, which generates points on the shape according to a probability density with respect to surface area on the shape, 
        the second one uses a density with respect to solid angle from the reference point. 
        This difference stems from the fact that the area light sampling routines evaluate the direct lighting integral as an integral over directions 
        from the reference point¡ªexpressing these sampling densities with respect to solid angle at the point is more convenient.
    */
    PBRT_CPU_GPU inline pstd::optional<ShapeSample> Sample(const ShapeSampleContext &ctx, /**/
                                                           Point2f u) const;

    /*
        It returns the shape¡¯s probability of sampling a point on the light such that the incident direction  at the reference point is wi. 
        As with the corresponding sampling method, this density should be with respect to solid angle at the reference point.
        As with the other Shape PDF() method, this should only be called for a direction that is known to intersect the shape from the reference point
    */
    PBRT_CPU_GPU inline Float PDF(const ShapeSampleContext &ctx, Vector3f wi) const;
};

}  // namespace pbrt

#endif  // PBRT_BASE_SHAPE_H
