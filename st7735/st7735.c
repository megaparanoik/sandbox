// SPDX-License-Identifier: GPL-2.0
#define DEBUG
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>

struct st7735_device {
	struct spi_device *spi;
	struct gpio_desc *dc;
	struct gpio_desc *reset;
};
struct st7735_device *st7735_dev;

//DC 1 - data / 0 - command
//RESET - 0 Active
//CS - 0 Active

static int st7735_send_cmd(struct st7735_device *st7735)
{

	return 0;
}

static int st7735_reset(struct st7735_device *st7735)
{
	gpiod_set_value_cansleep(st7735->reset, 0);
	msleep(100);
	gpiod_set_value_cansleep(st7735->reset, 1);
	msleep(120);

	return 0;
}

static int st7735_spi_probe(struct spi_device *spi)
{
	const struct spi_device_id *id = spi_get_device_id(spi);
	struct device *dev = &spi->dev;
	int ret;


	dev_info(&spi->dev, "Probing SPI device: %s\n", id->name);

	st7735_dev = devm_kzalloc(dev, sizeof(struct st7735_device), GFP_KERNEL);
	if (!st7735_dev)
		return -ENOMEM;

	st7735_dev->spi = spi;

	//Get reset pin
	st7735_dev->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(st7735_dev->reset)) {
		dev_err(dev, "Failed to get gpio reset\n");
		return PTR_ERR(st7735_dev->reset);
	}
	dev_info(&spi->dev, "Got Reset pin: %d\n", desc_to_gpio(st7735_dev->reset));

	//Get DC pin
	st7735_dev->dc = devm_gpiod_get(dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(st7735_dev->dc)) {
		dev_err(dev, "Failed to get gpio dc\n");
		return PTR_ERR(st7735_dev->dc);
	}
	dev_info(&spi->dev, "Got DC pin: %d\n", desc_to_gpio(st7735_dev->dc));

	spi->bits_per_word = 8;
	spi->mode = SPI_MODE_0;
	spi->max_speed_hz = 500000;
	ret = spi_setup(spi);
	if(ret < 0) {
		dev_err(dev, "Error initializing SPI: %d\n", ret);
		return ret;		
	}
	
	st7735_reset(st7735_dev);

	return 0;
}

static int st7735_spi_remove(struct spi_device *spi)
{
	return 0;
}


static const struct of_device_id st7735_of_match[] = {
	{ .compatible = "st7735", },
	{ },
};
MODULE_DEVICE_TABLE(of, st7735_of_match);


static const struct spi_device_id st7735_spi_id[] = {
	{ "st7735", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, st7735_spi_id);



static struct spi_driver st7735_spi_driver = {
	.driver = {
		.name = "st7735",
        .owner = THIS_MODULE,
		//.of_match_table = of_match_ptr(st7735_spi_dt_ids),
	},
	.probe = st7735_spi_probe,
	.remove = st7735_spi_remove,
	.id_table = st7735_spi_id,
};
module_spi_driver(st7735_spi_driver);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("st7735 SPI driver");
MODULE_VERSION("0.1");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");