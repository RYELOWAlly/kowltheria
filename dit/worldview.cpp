#include "worldview.hpp"
#include "ui_worldview.h"

#include <QPainter>

Worldview::Worldview(etheriasaves::EtheriaSave* save,
		     QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Worldview),
	rng(save->rng().world_seed())
{
	ui->setupUi(this);
	connect(ui->b_refresh, SIGNAL(clicked()),
		this, SLOT(refresh()));
	this->save = save;
	ui->c_mode->addItem("Biome Map");
	ui->c_mode->addItem("Height Map");
	ui->c_mode->addItem("Heat Map");
	ui->c_mode->setCurrentIndex(0);
	refresh();

}

Worldview::~Worldview()
{
	delete ui;
}

void Worldview::refresh()
{
	QLabel* l_view = ui->l_view;
	QImage image = QImage({360,180},
			      QImage::Format_RGB32);
	rng.seed = save->rng().world_seed();
	for(int longitude = -180; longitude < 180; longitude++)
	{
		for(int latitude = -90; latitude < 90; latitude++)
		{
			int graphicslong = (180) + longitude;
			int graphicslat  = (90)  + latitude;

			QRgb color = qRgb(255, 0, 255);

			if(ui->c_mode->currentText() == "Biome Map")
			{
				WorldBiome biome =
					rng.GetWorldBiomeAt(latitude,
							    longitude);
				switch(biome)
				{
				case BIOME_WATER:
					color = qRgb(0, 0, 127);
					break;
				case BIOME_DESERT:
					color = qRgb(255,255,0);
					break;
				case BIOME_BEACH:
					if(latitude % 2 == 0)
					{
						color = qRgb(0,0,0);
					}
					else
					{
						color = qRgb(200,255,100);
					}
					break;
				case BIOME_PLAINS:
					color = qRgb(127,255,127);
					break;
				case BIOME_HILLS:
					color = qRgb(200,255,200);
					break;
				case BIOME_JUNGLE:
					color = qRgb(0,155,0);
					break;
				case BIOME_FOREST:
					color = qRgb(0,255,0);
					break;
				case BIOME_ROCKBEACH:
					if((graphicslat +
					    graphicslong) % 4 == 0)
					{
						color = qRgb(15,15,45);
						break;
					}
					[[fallthrough]];
				case BIOME_ROCKPLAINS:
					if((graphicslat +
					    graphicslong) % 2 == 0)
					{
						color = qRgb(15,15,15);
						break;
					}
					[[fallthrough]];
				case BIOME_MOUNTAIN:
					color = qRgb(128,128,128);
					break;
				default:
					break;
				}
			}
			else if(ui->c_mode->currentText() == "Height Map")
			{
				double height =
					std::abs(rng.GetHeightAt(latitude,
								 longitude));
				int heightc = (height * 64 + 126);
				color = qRgb(heightc, heightc, heightc);
			}
			else if(ui->c_mode->currentText() == "Heat Map")
			{
				double heat =
					std::abs(rng.GetHeatAt(latitude,
							       longitude));
				int heightc = (heat * 64 + 126);
				color = qRgb(heightc, heightc, heightc);
			}
			if(latitude % 16 == 0)
				color = qRgb(0, 0, 0);
			image.setPixel(graphicslong, graphicslat,
				       color);
		}
	}
	l_view->setPixmap(QPixmap::fromImage(image));
}
