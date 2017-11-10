/** Copyright (C) 2016 Scott Lenser - Released under terms of the AGPLv3 License */

#ifndef UTILS_SPARSE_POINT_GRID_H
#define UTILS_SPARSE_POINT_GRID_H

#include <cassert>
#include <unordered_map>
#include <vector>
#include <list>

#include "intpoint.h"
#include "floatpoint.h"
#include "SparseGrid.h"

namespace cura {

/*! \brief Sparse grid which can locate spatially nearby elements efficiently.
 *
 * \tparam ElemT The element type to store.
 * \tparam Locator The functor to get the location from ElemT.  Locator
 *    must have: Point operator()(const ElemT &elem) const
 *    which returns the location associated with val.
 */
template<class ElemT, class Locator>
class SparsePointGrid : public SparseGrid<ElemT>
{
public:
    using Elem = ElemT;

    /*! \brief Constructs a sparse grid with the specified cell size.
     *
     * \param[in] cell_size The size to use for a cell (square) in the grid.
     *    Typical values would be around 0.5-2x of expected query radius.
     * \param[in] elem_reserve Number of elements to research space for.
     * \param[in] max_load_factor Maximum average load factor before rehashing.
     */
    SparsePointGrid(coord_t cell_size, size_t elem_reserve=0U, float max_load_factor=1.0f);

    /*! \brief Inserts elem into the sparse grid.
     *
     * \param[in] elem The element to be inserted.
     */
    void insert(const Elem &elem);

    /*!
     * TODO
     * 
     * \param[in] query_pt The point to search around.
     * \param[in] radius The search radius.
     * \return Vector of elements found
     */
    std::vector<Elem> getKnn(const FPoint3& query_pt, unsigned int k, coord_t radius) const;

protected:
    using GridPoint = typename SparseGrid<ElemT>::GridPoint;

    /*! \brief Accessor for getting locations from elements. */
    Locator m_locator;
};



#define SGI_TEMPLATE template<class ElemT, class Locator>
#define SGI_THIS SparsePointGrid<ElemT, Locator>

SGI_TEMPLATE
SGI_THIS::SparsePointGrid(coord_t cell_size, size_t elem_reserve, float max_load_factor)
 : SparseGrid<ElemT>(cell_size, elem_reserve, max_load_factor)
{
}


SGI_TEMPLATE
std::vector<typename SGI_THIS::Elem>
SGI_THIS::getKnn(const FPoint3 &query_pt, unsigned int k, coord_t radius) const
{
    struct DistElem
    {
        double dist2;
        Elem elem;
        DistElem(double dist2, const Elem& elem)
        : dist2(dist2)
        , elem(elem)
        {}
        bool operator<(const DistElem& b) const
        {
            return dist2 < b.dist2;
        }
    };
    std::list<DistElem> queue;
    using it = typename std::list<DistElem>::iterator;
    const std::function<bool (const Elem&)> process_func =
    [&query_pt, &queue, k, this](const Elem &elem)
    {
        double dist2 = (m_locator(elem) - query_pt).vSize2();
        bool inserted = false;
        for (it i = queue.begin(); i != queue.end(); ++i)
        {
            if (i->dist2 > dist2)
            {
                queue.insert(i, DistElem(dist2, elem));
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            if (queue.size() < k)
            {
                queue.emplace_back(dist2, elem);
            }
        }
        else
        {
            if (queue.size() > k)
            {
                queue.pop_back();
            }
        }
        return true;
    };
    SparseGrid<ElemT>::processNearby(query_pt, radius, process_func);
    std::vector<Elem> ret(k);
    for (int idx = 0; idx < k && !queue.empty(); idx++)
    {
        ret[idx] = queue.front().elem;
        queue.pop_front();
    }
    return ret;
}

SGI_TEMPLATE
void SGI_THIS::insert(const Elem &elem)
{
    FPoint3 loc = m_locator(elem);
    GridPoint grid_loc = SparseGrid<ElemT>::toGridPoint(loc);

    SparseGrid<ElemT>::m_grid.emplace(grid_loc,elem);
}


#undef SGI_TEMPLATE
#undef SGI_THIS

} // namespace cura

#endif // UTILS_SPARSE_POINT_GRID_H
