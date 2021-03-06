#include "TileStager.h"
#include "Queues/OverlayQueue.h"
#include "Queues/BackgroundQueue.h"
#include "Queues/TileQueue.h"
#include "Queues/ToolTileQueue.h"
#include "Renderer/Renderer.h"


TileStager::TileStager()
{
}

void TileStager::stageTiles(bool overlayOnToolbox)
{

    for (sf::RectangleShape& rect : BackgroundQueue::get().getQueue())
        Renderer::queueUI(&rect);

    tileManager.prepareTiles();


    for (sf::RectangleShape& rect : OverlayQueue::get().getQueue())
    {
        if (overlayOnToolbox)
            Renderer::queueUI(&rect);

        else
            Renderer::queueDrawable(&rect);
    }
}

void TileStager::clearAllQueues()
{
    ToolTileQueue::get().clear();
    TileQueue::get().clear();
    BackgroundQueue::get().clear();
    OverlayQueue::get().clear();
}