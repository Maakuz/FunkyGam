#include "TileStager.h"
#include "Queues/OverlayQueue.h"
#include "Queues/BackgroundQueue.h"
#include "Queues/TileQueue.h"
#include "Queues/ToolTileQueue.h"
#include "Queues/MiscQueue.h"
#include "Renderer/Renderer.h"


TileStager::TileStager()
{
}

void TileStager::stageTiles(bool overlayOnToolbox)
{

    for (sf::Drawable*& obj : BackgroundQueue::get().getQueue())
        Renderer::queueUI(obj);

    tileManager.prepareTiles();

    for (MiscQueue::Item& obj : MiscQueue::get().getQueue())
    {
        if (obj.isInterface)
            Renderer::queueUI(obj.ptr);

        else
            Renderer::queueDrawable(obj.ptr);
    }

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
    MiscQueue::get().clear();
}