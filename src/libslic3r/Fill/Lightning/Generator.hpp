//Copyright (c) 2021 Ultimaker B.V.
//CuraEngine is released under the terms of the AGPLv3 or higher.

#ifndef LIGHTNING_GENERATOR_H
#define LIGHTNING_GENERATOR_H

#include "Layer.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace Slic3r 
{
class PrintObject;

/*!
 * Generates the Lightning Infill pattern.
 *
 * The lightning infill pattern is designed to use a minimal amount of material
 * to support the top skin of the print, while still printing with reasonably
 * consistently flowing lines. It sacrifices strength completely in favour of
 * top surface quality and reduced print time / material usage.
 *
 * Lightning Infill is so named because the patterns it creates resemble a
 * forked path with one main path and many small lines on the side. These paths
 * grow out from the sides of the model just below where the top surface needs
 * to be supported from the inside, so that minimal material is needed.
 *
 * This pattern is based on a paper called "Ribbed Support Vaults for 3D
 * Printing of Hollowed Objects" by Tricard, Claux and Lefebvre:
 * https://www.researchgate.net/publication/333808588_Ribbed_Support_Vaults_for_3D_Printing_of_Hollowed_Objects
 */
class LightningGenerator  // "Just like Nicola used to make!"
{
public:
    /*!
     * Create a generator to fill a certain mesh with infill.
     *
     * This generator will pre-compute things in preparation of generating
     * Lightning Infill for the infill areas in that mesh. The infill areas must
     * already be calculated at this point.
     * \param mesh The mesh to generate infill for.
     */
    LightningGenerator(const PrintObject &print_object);

    /*!
     * Get a tree of paths generated for a certain layer of the mesh.
     *
     * This tree represents the paths that must be traced to print the infill.
     * \param layer_id The layer number to get the path tree for. This is within
     * the range of layers of the mesh (not the global layer numbers).
     * \return A tree structure representing paths to print to create the
     * Lightning Infill pattern.
     */
    const LightningLayer& getTreesForLayer(const size_t& layer_id) const;

protected:
    /*!
     * Calculate the overhangs above the infill areas that need to be supported
     * by infill.
     *
     * Normally, overhangs are only generated for the outside of the model and
     * only when support is generated. For this pattern, we also need to
     * generate overhang areas for the inside of the model.
     */
    void generateInitialInternalOverhangs(const PrintObject &print_object);

    /*!
     * Calculate the tree structure of all layers.
     */
    void generateTrees(const PrintObject &print_object);

    float m_infill_extrusion_width;

    /*!
     * How far each piece of infill can support skin in the layer above.
     */
    coord_t m_supporting_radius;

    /*!
     * How far a wall can support the wall above it. If a wall completely
     * supports the wall above it, no infill needs to support that.
     *
     * This is similar to the overhang distance calculated for support. It is
     * determined by the lightning_infill_overhang_angle setting.
     */
    coord_t m_wall_supporting_radius;

    /*!
     * How far each piece of infill can support other infill in the layer above.
     *
     * This may be different than \ref supporting_radius, because the infill is
     * printed with one end floating in mid-air. This endpoint will sag more, so
     * an infill line may need to be supported more than a skin line.
     */
    coord_t m_prune_length;

    /*!
     * How far a line may be shifted in order to straighten the line out.
     *
     * Straightening the line reduces material and time usage and reduces
     * accelerations needed to print the pattern. However it makes the infill
     * weak if lines are partially suspended next to the line on the previous
     * layer.
     */
    coord_t m_straightening_max_distance;

    /*!
     * For each layer, the overhang that needs to be supported by the pattern.
     *
     * This is generated by \ref generateInitialInternalOverhangs.
     */
    std::vector<Polygons> m_overhang_per_layer;

    /*!
     * For each layer, the generated lightning paths.
     *
     * This is generated by \ref generateTrees.
     */
    std::vector<LightningLayer> m_lightning_layers;
};

} // namespace Slic3r

#endif // LIGHTNING_GENERATOR_H
