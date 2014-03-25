#ifndef BOUNDED_EXPLORE_LAYER_H_
#define BOUNDED_EXPLORE_LAYER_H_
#include <ros/ros.h>
#include <costmap_2d/layer.h>
#include <costmap_2d/layered_costmap.h>
#include <costmap_2d/GenericPluginConfig.h>
#include <dynamic_reconfigure/server.h>
#include <costmap_2d/costmap_layer.h>

#include <geometry_msgs/Polygon.h>
#include <frontier_exploration/Frontier.h>
#include <frontier_exploration/UpdateBoundaryPolygon.h>
#include <frontier_exploration/GetNextFrontier.h>

namespace frontier_exploration
{

//class BoundedExploreLayer : public costmap_2d::Layer, public costmap_2d::Costmap2D
class BoundedExploreLayer : public costmap_2d::CostmapLayer
{
public:
    BoundedExploreLayer();
    ~BoundedExploreLayer();

    /**
     * @brief Loads default values and initialize exploration costmap.
     */
    virtual void onInitialize();
    virtual void updateBounds(double origin_x, double origin_y, double origin_yaw, double* polygon_min_x, double* polygon_min_y, double* polygon_max_x,
                              double* polygon_max_y);
    virtual void updateCosts(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j);
    bool isDiscretized()
    {
        return true;
    }

    virtual void matchSize();

    /**
     * @brief Reset exploration progress
     */
    virtual void reset();

    /**
     * @brief Load polygon boundary to draw on map with each update
     * @param polygon_stamped polygon boundary
     * @return true if successful
     */
    bool updateBoundaryPolygon(geometry_msgs::PolygonStamped polygon_stamped);

    /**
     * @brief Search the costmap for next reachable frontier to explore
     * @param start_pose Pose from which to start search
     * @param next_frontier Pose of found frontier
     * @return true if successful
     */
    bool getNextFrontier(geometry_msgs::PoseStamped start_pose, geometry_msgs::PoseStamped &next_frontier);

private:

    dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig> *dsrv_;
    ros::ServiceServer polygonService_;
    ros::ServiceServer frontierService_;
    geometry_msgs::Polygon polygon_;
    tf::TransformListener tf_listener_;

    ros::Publisher frontier_cloud_pub;

    bool configured_, marked_;

    std::string frontier_travel_point_;
    bool resize_to_boundary_;
    void reconfigureCB(costmap_2d::GenericPluginConfig &config, uint32_t level);

    /**
     * @brief ROS Service wrapper for updateBoundaryPolygon
     * @param req Service request
     * @param res Service response
     * @return true on service success, false otherwise
     */
    bool updateBoundaryPolygonService(frontier_exploration::UpdateBoundaryPolygon::Request &req, frontier_exploration::UpdateBoundaryPolygon::Response &res);

    /**
     * @brief Initialize costmap with exploration data, overwriting all but lethal obstacles from other layers
     * @param master_grid Reference to master costmap
     * @param min_i
     * @param min_j
     * @param max_i
     * @param max_j
     */
    void mapUpdateKeepObstacles(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j);

    /**
     * @brief ROS Service wrapper for getNextFrontier
     * @param req Service request
     * @param res Service response
     * @return true on service success, false otherwise
     */

    bool getNextFrontierService(frontier_exploration::GetNextFrontier::Request &req, frontier_exploration::GetNextFrontier::Response &res);

    /**
     * @brief findFrontiers Find all frontiers on costmap reachable from position
     * @param position Initial position
     * @param costmap Costmap to search
     * @return
     */
    std::list<Frontier> findFrontiers(geometry_msgs::Point position, costmap_2d::Costmap2D* costmap);

    /**
     * @brief Mark all frontier cells with indirect contact to candidate cell
     * @param idx index of candidate cell
     * @param robot Index of robot
     * @param map Reference to map data
     * @return Frontier structure containing infromation about the marked cells
     */
    Frontier buildFrontier(unsigned int initial_cell, unsigned int robot, bool* frontier_flag, const unsigned char* map);

    /**
      * @brief Evaluate if candidate cell is a valid frontier
      * @param idx index of candidate cell
      * @param map Reference to map data
      * @return True if frontier cell
      */
    bool isNewFrontierCell(unsigned int idx, bool* frontier_flag, const unsigned char* map);

    /**
     * @brief Find nearest cell of specified value
     * @param ret Result of search
     * @param idx Initial search cell
     * @param val Desired value
     * @param map Reference to map data
     * @return
     */
    bool nearestCell(unsigned int &result, unsigned int start, unsigned char val, const unsigned char* map);

    /**
    * @brief Determine 4-neighbourhood of an input cell, checking for map edges
    * @param idx input cell index
    * @return neighbour cell index
    */
    std::vector<unsigned int> nhood4(unsigned int idx);

    /**
     * @brief Determine 8-neighbourhood of an input cell, checking for map edges
     * @param idx input cell index
     * @return neighbour cell index
     */
    std::vector<unsigned int> nhood8(unsigned int idx);

};

}
#endif
